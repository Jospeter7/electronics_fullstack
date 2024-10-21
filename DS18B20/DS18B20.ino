#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#define DS18B20 8

LiquidCrystal_I2C lcd(0x27, 20, 4);
 
OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);

byte degree_symbol[8] =
{
0b00111,
0b00101,
0b00111,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000
};
void setup()
{
Serial.begin(9600);
delay(1000);
sensor.begin();
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temp: ");
}
 
void loop()
{
sensor.requestTemperatures();
Serial.print(sensor.getTempCByIndex(0));
Serial.println("°C");
lcd.setCursor(7,0);
lcd.print(sensor.getTempCByIndex(0));
lcd.write(1);
lcd.print("C");
delay(1000);
}