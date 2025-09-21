#include <ESP8266WiFi.h>

const char* ssid     = "ssid";
const char* password = "password";

void setup() {
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.println('\n');
  Serial.println("Connection established!");  

}

void loop() { }