#include<ESP8266WiFi.h>
void ConnectToWifi(void);  //function declaration/definition

void setup() {
  Serial.begin(115200);
  ConnectToWifi();
}

void loop() {
}

void ConnectToWifi (){
WiFi.mode(WIFI_STA); //nodemcu as station
WiFi.begin("********", "********"); //name of router(ssid) and password
Serial.print("connecting to wifi");
while(WiFi.status() !=WL_CONNECTED){
  Serial.print('.');
  delay(200);
}

Serial.print("IP Address:");
Serial.println(WiFi.localIP());  //this address can be changed
Serial.print("MacAddress:");
Serial.println(WiFi.macAddress());  //can not be changed
}
