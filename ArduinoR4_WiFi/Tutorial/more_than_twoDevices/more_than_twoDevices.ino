#include <WiFiS3.h>
#include "arduino_secrets.h"

WiFiClient client;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

void ConnectWiFi(); // Declare the ConnectWiFi function
void ThingSpeakWrite(float temperature, float voltage, float current, float power); // Update the declaration
void PrintNetwork(); // Declare the PrintNetwork function

void setup() 
{
  Serial.begin(9600);
  ConnectWiFi();
}

void loop() 
{
  // Generate random values for temperature, voltage, current, and calculate power
  float temperature = random(2000, 3000) / 100.0; // Simulated temperature in °C
  float voltage = random(3000, 5000) / 1000.0;    // Simulated voltage in V
  float current = random(500, 1500) / 1000.0;      // Simulated current in A
  float power = voltage * current;                  // Power in W
  
  // Print the values to the Serial Monitor
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Voltage: "); Serial.println(voltage);
  Serial.print("Current: "); Serial.println(current);
  Serial.print("Power: "); Serial.println(power);

  // Send the values to ThingSpeak
  ThingSpeakWrite(temperature, voltage, current, power); 
  delay(20000); // Delay between updates
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

void ThingSpeakWrite(float temperature, float voltage, float current, float power)
{
  char server[] = "api.thingspeak.com";
  unsigned long channelNumber = SECRET_CH_ID; // Ensure this is defined as a number
  String writeAPIKey = SECRET_WRITE_APIKEY;

  // Construct the POST data with all four fields
  String postData = "api_key=" + writeAPIKey + 
                    "&field1=" + String(voltage) + 
                    "&field2=" + String(current) + 
                    "&field3=" + String(power) +
                    "&field4=" + String(temperature);
  
  if (client.connect(server, 80)) {
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
