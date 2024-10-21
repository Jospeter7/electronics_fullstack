#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ezLED.h> // ezLED library

#define BUZZER D4
ezLED led(D3);  // create ezLED object that attach to pin 5


WiFiClient client;
HTTPClient http;
String API = "************";

RF24 radio(D0,D8);
const byte address[6] = "00001";

LiquidCrystal_I2C lcd(0x27, 20, 4);

byte degree_symbol[8] = {
    0b00111,
    0b00101,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};

struct sendData{
  float Turb;
  float Temp;
  float pH;
  float DO;
};


void initializeLCD();
void displayData(float Turb, float Temp, float pH, float DO);
float round_to_dp(float in_value, int decimal_place);
void ConnectToWifi();
void sendGETRequest(float Turb, float Temp, float pH, float DO);

void setup() {
    led.blink(500, 500);
    Serial.begin(9600);
    pinMode(BUZZER, OUTPUT); 
    initializeLCD();
    ConnectToWifi();
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
}

void loop() {
    led.loop();


    if(radio.available()){
      sendData data;
      radio.read(&data, sizeof(data));
      displayData(data.Turb, data.Temp, data.pH, data.DO);
      if (WiFi.status() == WL_CONNECTED) {
            sendGETRequest(data.Turb, data.Temp, data.pH, data.DO);
        }
      else {
            Serial.println("No WiFi connection, data not sent");
        }
        delay(1000);
    }
    else{
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("NO DATA RECEIVED");
      lcd.setCursor(4,1);
      lcd.print("PLEASE WAIT!");
      lcd.setCursor(2,2);
      lcd.print("AUTOCONFIGURING!");
      lcd.setCursor(4, 3);
      lcd.print("THANK YOU!");
      delay(1000);
    }
     
}

void initializeLCD() {
    lcd.init();
    lcd.backlight();
    lcd.createChar(1, degree_symbol);
    lcd.clear();
}

void displayData(float Turb, float Temp, float pH, float DO) {
    lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("TURB :   ");
    // lcd.print(Turb);
    // lcd.print(" ntu");

    lcd.setCursor(0, 0);
    lcd.print("TEMP :   ");
    lcd.print(Temp);
    lcd.write(1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    if(pH<0 && pH>14){
      lcd.print("PH:");
      lcd.print("INVALID");
    }
    else{
      lcd.print("PH :     ");
      lcd.print(pH);
    }
    
    lcd.setCursor(0, 2);
    lcd.print("DO :     ");
    lcd.print(DO);
    lcd.print(" mg/L");
    if (DO < 2.0) {  
            digitalWrite(BUZZER, LOW);  
        } 
    else {
            digitalWrite(BUZZER, HIGH); 
        }
}

void ConnectToWifi() {WiFi.mode(WIFI_STA);
    WiFi.begin("wifi name", "password"); 
    //Serial.print("CONNECTING TO INTERNET");
    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        lcd.setCursor(4, 0);
        lcd.print(" CONNECTING");
        lcd.setCursor(5, 1);
        lcd.print(" TO THE");
        lcd.setCursor(5, 2);
        lcd.print(" INTERNET");
        delay(500);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Mac Address: ");
        Serial.println(WiFi.macAddress());
    } else {
    }
}

void sendGETRequest(float Turb, float Temp, float pH, float DO) {
    String url = "http://api.thingspeak.com/update?";
    url += "api_key=" + API;
    url += "&field1=" + String(Temp);
    url += "&field2=" + String(Turb);
    url += "&field3=" + String(pH);
    url += "&field4=" + String(DO);
    http.begin(client, url);
    Serial.println("data sent");
    int httpCode = http.GET();
}


