#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

char ssid[] = "ssid";
char pass[] = "password";

WiFiClient client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "A";

void setup() {
  Serial.begin(115200);
  ThingSpeak.begin(client);
  Serial.print("Setup!");
}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected!");
  }

  float fakeTemp = 25.0;
  float fakeHum  = 50.0;
  float fakePres = 1000.0;

  ThingSpeak.setField(1, fakeTemp);
  ThingSpeak.setField(2, fakeHum);
  ThingSpeak.setField(3, fakePres);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(20000);
}
