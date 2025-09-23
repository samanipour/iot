#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BH1750.h>
#include <MQ135.h>
#include "ThingSpeak.h"
#include <TFT_eSPI.h>

MAX30105 HeartSensor;
float beatsPerMinute = 0;   
int beatAvg = 0;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; 

BH1750 lightMeter(0x23);

TFT_eSPI tft = TFT_eSPI();
int cursorX = 10;     // فاصله از چپ
int cursorY = 10;     // موقعیت y شروع
int lineHeight = 28;  

#define PIN_MQ135 A0
MQ135 mq135_sensor(PIN_MQ135);

WiFiClient client;
unsigned long myChannelNumber = 3084748;
const char * myWriteAPIKey = "G6WHW2R879PU2A7D";

bool Connection_setup(String ssid, String password) {
  int maxRetries = 5;
  int retryCount = 0;
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(2000);
    retryCount++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  else {
    return true;
  }
}

bool Connection_loop() {
  return WiFi.status() == WL_CONNECTED;
}

bool MAX30105_setup() {
  // Initialize sensor
  if (!HeartSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    return false;
  }
  HeartSensor.setup(); //Configure sensor with default settings
  HeartSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  HeartSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  return true;
}

// long MAX30105_loop() {
//   long irValue = HeartSensor.getIR();

//   if (checkForBeat(irValue) == true)
//   {
//     //We sensed a beat!
//     long delta = millis() - lastBeat;
//     lastBeat = millis();

//     beatsPerMinute = 60 / (delta / 1000.0);

//     if (beatsPerMinute < 255 && beatsPerMinute > 20)
//     {
//       rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
//       rateSpot %= RATE_SIZE; //Wrap variable

//       //Take average of readings
//       beatAvg = 0;
//       for (byte x = 0 ; x < RATE_SIZE ; x++)
//         beatAvg += rates[x];
//       beatAvg /= RATE_SIZE;
//     }
//     if (irValue < 50000)
//       return -1;
//     return beatsPerMinute;
//   }
// }

// long MAX30105_loop() {
//   long irValue = HeartSensor.getIR();
//   bool beatDetected = checkForBeat(irValue);

//   long delta = millis() - lastBeat;
//   lastBeat = beatDetected ? millis() : lastBeat;
//   beatsPerMinute = beatDetected ? (60 / (delta / 1000.0)) : beatsPerMinute;

//   if (beatDetected && beatsPerMinute < 255 && beatsPerMinute > 20) {
//     rates[rateSpot++] = (byte)beatsPerMinute;
//     rateSpot %= RATE_SIZE;

//     beatAvg = 0;
//     for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
//     beatAvg /= RATE_SIZE;
//   }

//   if (irValue < 50000) return -1;

//   return beatAvg;
// }

bool BH1750_setup() {

  if (!lightMeter.begin()) {
    return false;
  }
  return true;

}

float BH1750_loop() {
  float lux = lightMeter.readLightLevel();
  return lux;
}

bool TFT_setup() {
  tft.begin();
  tft.setSwapBytes(true);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.println("Loading...");

  delay(1000);

  return true;
}

void TFT_loop() {
  tft.fillScreen(TFT_RED);
}

void printLine(String text, int size) {
  tft.setTextSize(size);
  tft.drawString(text, cursorX, cursorY);
  cursorY += lineHeight;  // y خط بعد رو افزایش میده
  // اگر رسیدیم به پایین صفحه، برگردیم بالا
  if(cursorY + lineHeight > tft.height()) {
    tft.fillScreen(TFT_BLACK);
    cursorY = 10;
  }
}

long MQ135_loop() {
  float ppm = mq135_sensor.getPPM();
  return ppm;
}

void Print_status(bool func, String name) {
  if (!func) {
    Serial.println(name + "Failed!");
  } else {
    Serial.println(name + "initialized.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin(D2, D1);  // SDA, SCL

  Print_status(TFT_setup(), "TFT Display");


  if (!Connection_setup("Clone", "1qazxsw2")) {
    Serial.println("WiFi Connection Failed!");
  } else {
    Serial.println("WiFi Connected!");
  }

  Print_status(MAX30105_setup(), "MAX30105");

  Print_status(BH1750_setup(), "BH1750");

  Print_status(TFT_setup(), "TFT Display");

  ThingSpeak.begin(client);


}

void loop() {

  if (!Connection_loop()) {
    Serial.println("WiFi Disconnected!");
  }

  long bpm = HeartSensor.getIR();
  Serial.println("BPM: " + String(bpm));

  float lux = BH1750_loop();
  Serial.println("Lux: " + String(lux));

  float ppm = MQ135_loop();
  Serial.println("PPM: " + String(ppm));

  ThingSpeak.setField(1, ppm);
  ThingSpeak.setField(2, lux);
  ThingSpeak.setField(3, bpm);


  
  tft.fillScreen(TFT_BLACK);
  cursorY = 10;  // بازنشانی موقعیت y به بالا

   
  printLine("PPM:",1);
  printLine(String(ppm, 2),2);
  printLine("LUX:",1);
  printLine(String(lux, 2),2);
  printLine("Heartrate:",1);
  printLine(String(bpm),2);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  // if(x == 200){
  //   Serial.println("Channel update successful.");
  // }
  // else{
  //   Serial.println("Problem updating channel. HTTP error code " + String(x));
  // }

  delay(1500);
}