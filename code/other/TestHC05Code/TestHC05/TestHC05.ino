#include<SoftwareSerial.h>

char receivedValueBT;

/* Create object named bt of the class SoftwareSerial */ 
SoftwareSerial bt(10,11); /* (Rx,Tx) */	

void setup() {
  bt.begin(9600);	/* Define baud rate for software serial communication */
  Serial.begin(9600);	/* Define baud rate for serial communication */
}

void loop() {
  
    if (bt.available()) 
  {
    receivedValueBT = bt.read();
    if (receivedValueBT == '1')
      {
        Serial.write("One");
      }
    else if (receivedValueBT == '2')
      {
        Serial.write("Two");
      }
    else if (receivedValueBT == '3')
      {
        Serial.write("Three");
      }
    else if (receivedValueBT == '4')
      {
        Serial.write("Four");
      }
    else if (receivedValueBT == '5')
      {
        Serial.write("Five");
      }
    else if (receivedValueBT == '6')
      {
        Serial.write("Six");
      }
    else if (receivedValueBT == '7')
      {
        Serial.write("Seven");
      }
    else if (receivedValueBT == '8')
      {
        Serial.write("Eight");
      }
    else if (receivedValueBT == '9')
      {
        Serial.write("Nine");
      }

  }
  delay(20);
}
