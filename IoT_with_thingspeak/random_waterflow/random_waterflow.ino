#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

WiFiClient client;
HTTPClient http;
String API = "***********";
String flowRateField = "1";
String totalLitresField = "2";

void ConnectToWifi(); 

void setup() {
  Serial.begin(115200);
  ConnectToWifi();
}

void loop() {
  int flowRate = random(0, 100);
  int totalLitres = random(0, 1000);

  sendGETRequest(flowRate, totalLitres);
  delay(16000);
}

void sendGETRequest(int flowRate, int totalLitres) {
  String url = "http://api.thingspeak.com/update?";
  url += "api_key=" + API;
  url += "&field1=" + String(flowRate);
  url += "&field2=" + String(totalLitres);

  http.begin(client, url);
  Serial.println("Waiting for Response");
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("Data Sent Successfully");
  } else {
    Serial.println("Error in Sending");
  }
  http.end();
}

void ConnectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("********", "*********");
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
}
