#include <Servo.h> 

int servoPin = 3; 

Servo Servo1; 

int incomingByte = 0;
int angle = 0;

void setup() {
  Serial.begin(9600);
  
  Servo1.attach(servoPin); 
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    if(incomingByte != 10){
      if(incomingByte == 65){//65 = A
        Serial.print("Angle: ");
        Serial.println(angle);
        
        Servo1.write(angle); 
        
        angle = 0;
        Serial.println("Angle sent to servo");
        Serial.println("###############################");
      }
      else{
        if(angle > 180){
          angle = 0;
        }
        incomingByte -= 48;
        Serial.print("I received: ");
        Serial.println(incomingByte);
        angle += incomingByte;
        Serial.print("Angle: ");
        Serial.println(angle);
        Serial.println("Press 'A' to write to servo");
        Serial.println("-------------------------------");
      }
    }
  }
}
