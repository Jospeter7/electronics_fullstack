#include <WiFiS3.h>
#include "arduino_secrets.h"

WiFiClient client;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

void ConnectWiFi(); // Declare the ConnectWiFi function
void ThingSpeakWrite(float fieldValue); // Declare the ThingSpeakWrite function
void PrintNetwork(); // Declare the PrintNetwork function

void setup() 
{
  Serial.begin(9600);
  ConnectWiFi();
}

void loop() 
{
  float temperature;
  temperature = random(2000, 3000) / 100.0;
  Serial.println(temperature);
  ThingSpeakWrite(temperature);  // This function should now be recognized
  delay(20000);
}

void ConnectWiFi()
{
  // Check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection:
    delay(10000);
  }

  // You're connected now, so print out the data:
  Serial.println("You're connected to WiFi");
  PrintNetwork();
}

void PrintNetwork() 
{
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void ThingSpeakWrite(float fieldValue)
{
  char server[] = "api.thingspeak.com";
  unsigned long channelNumber = SECRET_CH_ID;
  String writeAPIKey = SECRET_WRITE_APIKEY;
  int channelField = 1;
  
  if (client.connect(server, 80)) {
    String postData = "api_key=" + writeAPIKey + "&field" + String(channelField) + "=" + String(fieldValue);
    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postData.length()));
    client.println();
    client.println(postData);
  } else {
    Serial.println("Connection Failed");
  }
}
