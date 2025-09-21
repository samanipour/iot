#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const char* ssid     = "ssid";
const char* password = "password";

long lastBeat = 0;
float beatsPerMinute;   
int beatAvg;

int maxRetries = 5;
int retryCount = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(2000);
    retryCount++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection failed!");
  }
  else {
    Serial.println("Connection established!");
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 not found.");
    while (1);
  }

  particleSensor.setup();

}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      beatAvg = (beatAvg * 0.9) + (beatsPerMinute * 0.1);
    }

    Serial.print("BPM: ");
    Serial.println(beatAvg);
  }

}