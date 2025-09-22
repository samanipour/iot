#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BH1750.h>

MAX30105 HeartSensor;
long lastBeat = 0;
float beatsPerMinute = 0;   
int beatAvg = 0;

BH1750 lightMeter;

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
  if (!HeartSensor.begin(Wire, I2C_SPEED_FAST)) {
    return false;
  }

  HeartSensor.setup();
  return true;
}

long MAX30105_loop() {
  long irValue = HeartSensor.getIR();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      beatAvg = (beatAvg * 0.9) + (beatsPerMinute * 0.1);
    }
    return beatAvg;
  }
  else {
    return -1;
  }
}

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

  if (!Connection_setup("ssid", "password")) {
    Serial.println("WiFi Connection Failed!");
  } else {
    Serial.println("WiFi Connected!");
  }

  Print_status(MAX30105_setup(), "MAX30105");

  Print_status(BH1750_setup(), "BH1750");

}

void loop() {

  if (!Connection_loop()) {
    Serial.println("WiFi Disconnected!");
  }

  // long bpm = MAX30105_loop();
  // if(bpm != -1) Serial.println("BPM: " + String(bpm));

  float lux = BH1750_loop();
  Serial.println("Lux: " + String(lux));

  delay(1000);

}