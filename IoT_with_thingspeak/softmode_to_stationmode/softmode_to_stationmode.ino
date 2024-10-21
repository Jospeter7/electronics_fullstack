#include<ESP8266WiFi.h>
#include<DNSServer.h>
#include<WiFiManager.h>

WiFiManager wifi;
void setup() {
  Serial.begin(9600);
  wifi.autoConnect("**********");
  Serial.println("Connected to *********");
  // put your setup code here, to run once:

}

void loop() {

}
