#include<ESP8266WiFi.h>
#include<WiFiClient.h>
#include<ESP8266HTTPClient.h>

WiFiClient client;
HTTPClient http;
String url;
String API = "*********";
String FieldNo = "1";

void ConnectToWifi(void);  //function declaration/definition

void setup() {
  Serial.begin(115200);
  ConnectToWifi();

}
int httpcode, stepvalue;
//https://api.thingspeak.com/update?api_key=************M&field1=0

void loop() {
  stepvalue = analogRead(A0);
  sendGETRequest(stepvalue);
delay(16000);
}

void sendGETRequest(int data){
  url = "http://api.thingspeak.com/update?api_key=";
  url = url + API;
  url = url + "&field";
  url = url + FieldNo;
  url = url + "=";
  url = url + data;
  http.begin(client, url);
  Serial.println("Waiting for Response");
  httpcode = http.GET();
  if(httpcode > 0){
    Serial.print(stepvalue);
    Serial.println("- Data Sent Successifuly");
  }
  else{
    Serial.println("Error in Sending");
  }
http.end();
}

void ConnectToWifi (){
WiFi.mode(WIFI_STA); //nodemcu as station
WiFi.begin("name", "password"); //name of router(ssid) and password
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

