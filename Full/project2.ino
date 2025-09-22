#include <ESP8266WiFi.h>
#include <Wire.h>
// #include "MAX30105.h"
// #include "heartRate.h"
#include <BH1750.h>
#include <MQ135.h>
#include "ThingSpeak.h"
#include <TFT_eSPI.h>

// MAX30105 HeartSensor;
// long lastBeat = 0;
// float beatsPerMinute = 0;   
// int beatAvg = 0;

BH1750 lightMeter;

TFT_eSPI tft = TFT_eSPI();
int cursorX = 10;     // فاصله از چپ
int cursorY = 10;     // موقعیت y شروع
int lineHeight = 35;  

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

// bool MAX30105_setup() {
//   if (!HeartSensor.begin(Wire, I2C_SPEED_FAST)) {
//     return false;
//   }

//   HeartSensor.setup();
//   return true;
// }

// long MAX30105_loop() {
//   long irValue = HeartSensor.getIR();

//   if (checkForBeat(irValue) == true) {
//     long delta = millis() - lastBeat;
//     lastBeat = millis();

//     beatsPerMinute = 60 / (delta / 1000.0);

//     if (beatsPerMinute < 255 && beatsPerMinute > 20) {
//       beatAvg = (beatAvg * 0.9) + (beatsPerMinute * 0.1);
//     }
//     return beatAvg;
//   }
//   else {
//     return -1;
//   }
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
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

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

  if (!Connection_setup("Clone", "1qazxsw2")) {
    Serial.println("WiFi Connection Failed!");
  } else {
    Serial.println("WiFi Connected!");
  }

  // Print_status(MAX30105_setup(), "MAX30105");

  Print_status(BH1750_setup(), "BH1750");

  Print_status(TFT_setup(), "TFT Display");

  ThingSpeak.begin(client);


}

void loop() {

  if (!Connection_loop()) {
    Serial.println("WiFi Disconnected!");
  }

  // long bpm = MAX30105_loop();
  // if(bpm != -1) Serial.println("BPM: " + String(bpm));

  float lux = BH1750_loop();
  Serial.println("Lux: " + String(lux));

  TFT_loop();

  float ppm = MQ135_loop();
  Serial.println("PPM: " + String(ppm));

  ThingSpeak.setField(1, ppm);
  ThingSpeak.setField(2, lux);

  
  tft.fillScreen(TFT_BLACK);
  cursorY = 10;  // بازنشانی موقعیت y به بالا

   
  printLine("PPM:",2);
  printLine(String(ppm, 2),3);
  printLine("LUX:",2);
  printLine(String(lux, 2),3);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(20000);
}