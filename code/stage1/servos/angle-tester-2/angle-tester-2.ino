#include "HCPCA9685.h"

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

void setup() {
  Serial.begin(9600);
  
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);
  
  HCPCA9685.Servo(6, map(0, 0, 180, 1, 450));
  HCPCA9685.Servo(7, map(150, 0, 180, 1, 450));
  
  Serial.println("Init"); 
}

void loop() {
}
