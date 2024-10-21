#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>  
#include <Arduino.h>

RF24 radio(9,10);
const byte address[6] = "00001";


#define DS18B20 A3
#define TURBIDITY A2
#define pH_sensor A1
#define DO_sensor A0
#define LIGHT_SENSOR_PIN A4
#define LED_PIN 5
#define ANALOG_THRESHOLD 500
#define DEBOUNCE_SAMPLES 10 

#define VREF 5000    //VREF (mv)
#define ADC_RES 1024 //ADC Resolution
 
//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 0
 
#define READ_TEMP (25) //Current water temperature ℃, Or temperature sensor function
 
//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (1259) //mv
#define CAL1_T (25)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃
 
const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};
 
uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;
 
int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 00
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

float calibration_value = 21.34+0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10],temp;

#define VREF    5000 // VREF(mV)
#define ADC_RES 1024 // ADC Resolution

OneWire ourWire(DS18B20);
DallasTemperature tempSensor(&ourWire);
Servo myservo;
 

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

void displayData(float temperatureC, float volt, float ntu, float pH_val, float do_val);
float round_to_dp(float in_value, int decimal_place);
float readTemperature();
float readVoltage();
float calculateNTU(float volt);
float readPH();
float readOxy();

struct sendData{
  float Turb;
  float Temp;
  float pH;
  float DO;
};

void setup() {
    Serial.begin(9600);
    tempSensor.begin();

    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.stopListening();
    pinMode(LED_PIN, OUTPUT);
    myservo.attach(6,1000,2000);
}

void loop() {
  
    int analogValue = 0;
    int stableValue = 0;
    int potValue; 

    potValue = analogRead(A5);   
    potValue = map(potValue, 0, 1023, 0, 180);   
    myservo.write(potValue);
  
    for (int i = 0; i < DEBOUNCE_SAMPLES; i++) {
        analogValue += analogRead(LIGHT_SENSOR_PIN);
        delay(10); 
    }
    stableValue = analogValue / DEBOUNCE_SAMPLES; 

    if (stableValue < ANALOG_THRESHOLD) {
        digitalWrite(LED_PIN, HIGH); 
    } else {
        digitalWrite(LED_PIN, LOW);  
    }
    float temperatureC = readTemperature();
    float volt = readVoltage();
    float ntu = calculateNTU(volt);
    float pH_val = readPH();
    float do_val = readOxy();

    sendData data = {ntu, temperatureC, pH_val, do_val};
    bool success = radio.write(&data, sizeof(data));

    if (success) {
        Serial.println("Data sent");
    } else {
        Serial.println("Error");
    }
}

float readTemperature() {
    tempSensor.requestTemperatures();
    return tempSensor.getTempCByIndex(0);
}


float readVoltage() {
    float volt = 0;
    for (int i = 0; i < 800; i++) {
        volt += ((float)analogRead(TURBIDITY) / 1023) * 5;
    }
    return round_to_dp(volt / 800, 2);
}


float calculateNTU(float volt) {
    if (volt < 2.5) {
        return 3000;
    } else {
        return -1120.4 * sq(volt) + 5742.3 * volt - 4353.8;
    }
}

float readPH(){
  for(int i=0;i<10;i++)
{
buffer_arr[i]=analogRead(pH_sensor);
delay(30);
}
for(int i=0;i<9;i++)
{
for(int j=i+1;j<10;j++)
{
if(buffer_arr[i]>buffer_arr[j])
{
temp=buffer_arr[i];
buffer_arr[i]=buffer_arr[j];
buffer_arr[j]=temp;
}
}
}
avgval=0;
for(int i=2;i<8;i++)
avgval+=buffer_arr[i];
float PH_volt=(float)avgval*5.0/1024/6;
return -5.70 * PH_volt + calibration_value;
}

float readOxy() {
Temperaturet = (uint8_t)READ_TEMP;
  ADC_Raw = analogRead(DO_sensor);
  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;
 
  //Serial.print("Temperaturet:\t" + String(Temperaturet) + "\t");
  //Serial.print("ADC RAW:\t" + String(ADC_Raw) + "\t");
  //Serial.print("ADC Voltage:\t" + String(ADC_Voltage) + "\t");
  return ((readDO(ADC_Voltage, Temperaturet))/1000);
}

float round_to_dp(float in_value, int decimal_place) {
    float multiplier = pow(10.0f, decimal_place);
    in_value = round(in_value * multiplier) / multiplier;
    return in_value;
}



