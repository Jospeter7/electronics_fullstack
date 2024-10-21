#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 4); //SIM800L Tx & Rx is connected to Arduino #3 & #2
String cmd = "";

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing...");
  delay(50);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); //Check whether it has registered in the network
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(50);
  while (Serial.available()) 
  {
    cmd += (char)Serial.read();//Forward what Serial received to Software Serial Port
    cmd.trim();
    if (cmd.equals("S")) {
      sendSMS();
    }else{
      mySerial.print(cmd);
      mySerial.println("");
    }
  }
 while (mySerial.available()) {
   Serial.write(mySerial.read());
 }
}

void sendSMS() {
  mySerial.println("AT+CMGS=\"+255*********\"\r");
  delay(50);
  mySerial.println("Hi there");
  delay(50);
  mySerial.write(26);
}