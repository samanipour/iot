#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 HeartSensor;
long lastBeat = 0;
float beatsPerMinute = 0;   
int beatAvg = 0;

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

void setup() {
  Serial.begin(115200);
  delay(100);

  if (!Connection_setup("ssid", "password")) {
    Serial.println("WiFi Connection Failed!");
  } else {
    Serial.println("WiFi Connected!");
  }

  if (!MAX30105_setup()) {
    Serial.println("MAX30105 not found!");
  }
  else {
    Serial.println("MAX30105 initialized.");
  }
}

void loop() {

  if (!Connection_loop()) {
    Serial.println("WiFi Disconnected!");
  }

}