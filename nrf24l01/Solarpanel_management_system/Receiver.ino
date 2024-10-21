#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ezLED.h> 

ezLED led(A2);  

RF24 radio(9,10);
const byte address[6] = "00002";

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
  float Volt;
  float Cur;
  float Ligh;
};

void initializeLCD();
void displayData(float Volt, float Cur, float Ligh);
float round_to_dp(float in_value, int decimal_place);

void setup() {
    led.blink(500, 500);
    Serial.begin(9600);
    initializeLCD();
   
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
      displayData(data.Volt, data.Cur, data.Ligh);
      delay(1000);
    }
    else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  NO DATA RECEIVED");
      lcd.setCursor(0,1);
      lcd.print("    PLEASE WAIT!");
      lcd.setCursor(0,2);
      lcd.print("  AUTOCONFIGURING!");
      lcd.setCursor(0,3);
      lcd.print("    THANK YOU!");
      delay(1000);
    }
     
}

void initializeLCD() {
    lcd.init();
    lcd.backlight();
    lcd.createChar(1, degree_symbol);
    lcd.clear();
}

void displayData(float Volt, float Cur, float Ligh) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VOLTAGE: ");
    lcd.print(Volt);
    lcd.print(" V");

    lcd.setCursor(0, 1);
    lcd.print("CURRENT: ");
    lcd.print(Cur);
    lcd.print(" A");

    lcd.setCursor(0, 2);
    lcd.print("LIGHT:   ");
    lcd.print(Ligh);
    lcd.print(" L");

    lcd.setCursor(0, 3);
    lcd.print("POWER:   ");
    lcd.print(Volt*Cur);
    lcd.print(" W");
}



