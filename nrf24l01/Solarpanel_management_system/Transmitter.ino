#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ezLED.h> 

ezLED led(5);  

#define CURRENT A0
#define VOLTAGE A1
#define LIGHT A2
#define LED_PIN 7


RF24 radio(9,10);
const byte address[6] = "00002";


float round_to_dp(float in_value, int decimal_place);
float readVoltage();
float readCurrent();
float readLight();

struct sendData{
  float Volt;
  float Cur;
  float Ligh;
};

void setup() {
    Serial.begin(9600);
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.stopListening();

     pinMode(LED_PIN, OUTPUT);

    led.blink(1000, 500);
}

void loop() {

    led.loop();
    float volts= readVoltage();
    float currents = readCurrent();
    float light = readLight();
    sendData data = {volts, currents, light};
    bool success = radio.write(&data, sizeof(data)); 

    if (light <= 20) {
        digitalWrite(LED_PIN, HIGH); 
    } 
    else {
        digitalWrite(LED_PIN, LOW);  
    }
    
    if(success){
      Serial.println("data sent");
    }
    else{
      Serial.println("Error");
    }
}

float readVoltage(){
  // Floats for ADC voltage & Input voltage
  float adc_voltage = 0.0;
  float in_voltage = 0.0;
 
  // Floats for resistor values in divider (in ohms)
  float R1 = 390000.0;
  float R2 = 10000.0; 
 
  // Float for Reference Voltage
  float ref_voltage = 5.0;
 
  // Integer for ADC value
  int adc_value = 0;
  adc_value = analogRead(VOLTAGE);
  adc_voltage  = (adc_value * ref_voltage) / 1023.0;
  return adc_voltage*(R1+R2)/R2;
}

float readCurrent(){
    unsigned int x=0;
    float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

    for (int x = 0; x < 150; x++){ //Get 150 samples
    AcsValue = analogRead(A0);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delay (3); // let ADC settle before next sample 3ms
    }
    AvgAcs=Samples/149.2;//Taking Average of Samples

    return (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.066;
}

float readLight(){
  float volts = analogRead(LIGHT) * 5.0 / 1024.0;
  float amps = volts / 10000.0; // across 10,000 Ohms
  float microamps = amps * 1000000;
  return microamps * 2.0;
}

float round_to_dp(float in_value, int decimal_place) {
    float multiplier = pow(10.0f, decimal_place);
    in_value = round(in_value * multiplier) / multiplier;
    return in_value;
}