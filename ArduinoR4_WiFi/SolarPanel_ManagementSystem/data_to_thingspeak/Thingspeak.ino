#include <WiFiS3.h>
#include "arduino_secrets.h"
#include <ezLED.h> 

WiFiClient client;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

#define CURRENT A0
#define VOLTAGE A1
//#define LIGHT A2
//#define LED_PIN 7

//ezLED led(5);  

void ConnectWiFi(); // Declare the ConnectWiFi function
//void ThingSpeakWrite(float temperature, float voltage, float current, float light); // Update declaration
void ThingSpeakWrite(float voltage, float current); // Update declaration
void PrintNetwork(); // Declare the PrintNetwork function

void setup() 
{
  Serial.begin(9600);
  ConnectWiFi();
  //pinMode(LED_PIN, OUTPUT);
  //led.blink(1000, 500);
}

void loop() 
{
  // Generate random temperature value
  //float temperature = random(2000, 3000) / 100.0; // Simulated temperature in °C
  float voltage = readVoltage();
  float current = readCurrent();
  //float light = readLight();

  // Print sensor values to the Serial Monitor
  //Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Voltage: "); Serial.println(voltage);
  Serial.print("Current: "); Serial.println(current);
  //Serial.print("Light: "); Serial.println(light);

  // Send the values to ThingSpeak
  //ThingSpeakWrite(temperature, voltage, current, light); 
  ThingSpeakWrite(voltage, current);

  // // Control LED based on light sensor reading
  // if (light <= 20) {
  //   digitalWrite(LED_PIN, HIGH); 
  // } else {
  //   digitalWrite(LED_PIN, LOW);  
  // }

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

//void ThingSpeakWrite(float temperature, float voltage, float current, float light)
void ThingSpeakWrite(float voltage, float current)
{
  char server[] = "api.thingspeak.com";
  unsigned long channelNumber = SECRET_CH_ID; // Ensure this is defined as a number
  String writeAPIKey = SECRET_WRITE_APIKEY;

  // Construct the POST data with all four fields
  String postData = "api_key=" + writeAPIKey + 
                    "&field1=" + String(voltage) + 
                    "&field2=" + String(current); 
                    //"&field3=" + String(power) + 
                    // "&field4=" + String(light);
  
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

float readVoltage() {
  float adc_voltage = 0.0;
  float in_voltage = 0.0;
  float R1 = 390000.0; // Resistor values for voltage divider
  float R2 = 10000.0; 
  float ref_voltage = 5.0; // Reference Voltage
  int adc_value = analogRead(VOLTAGE);
  adc_voltage = (adc_value * ref_voltage) / 1023.0;
  return adc_voltage * (R1 + R2) / R2; // Calculate actual voltage
}

float readCurrent() {
  unsigned int x = 0;
  float AcsValue = 0.0, Samples = 0.0, AvgAcs = 0.0;

  for (int x = 0; x < 150; x++) { // Get 150 samples
    AcsValue = analogRead(CURRENT); // Read current sensor values   
    Samples += AcsValue;  // Add samples together
    delay(3); // Let ADC settle before next sample
  }
  AvgAcs = Samples / 149.0; // Taking Average of Samples
  return (2.5 - (AvgAcs * (5.0 / 1024.0))) / 0.066; // Calculate current
}

// float readLight() {
//   float volts = analogRead(LIGHT) * 5.0 / 1024.0;
//   float amps = volts / 10000.0; // across 10,000 Ohms
//   float microamps = amps * 1000000;
//   return microamps * 2.0; // Calculate light
// }
