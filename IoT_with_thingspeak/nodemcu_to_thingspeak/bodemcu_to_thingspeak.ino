#include<ESP8266WiFi.h>
#include<WiFiClient.h>
#include<ESP8266HTTPClient.h>

WiFiClient client;
HTTPClient http;
void ConnectToWifi(void);  //function declaration/definition

void setup() {
  Serial.begin(115200);
  ConnectToWifi();

}
int httpcode;
//https://api.thingspeak.com/update?api_key=**************&field1=0

void loop() {
  http.begin(client, "http://api.thingspeak.com/update?api_key=*************&field1=456");
  Serial.println("Waiting for Response");
  httpcode = http.GET();
  if(httpcode > 0){
    Serial.println("Data Sent Successifuly");
  }
  else{
    Serial.println("Error in Sending");
  }
http.end();
delay(16000);
}

void ConnectToWifi (){
WiFi.mode(WIFI_STA); //nodemcu as station
WiFi.begin("************", "************"); //name of router(ssid) and password
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

