#include <Arduino.h>
//--- for mac addres obtaining
#include <WiFi.h>
//---

//--- for walking
#include "HCPCA9685.h"
#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 150, 35, 0}, {3, 120, 85, 135, 75, 0}, {0, 115, 90, 150, 85, 0}, {0, 115, 100, 160, 90, 0}};
//!!! the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

double x[4] = {0 ,0, 0, 0};
double height = 1;
double y[4] = {0 ,0, 0, 0};

//all functions predeclaration
// complex movements
void walk(int speed);
void walkTrust(int speed);
void walkReverse(int speed);
// basic movements
void standDown();
void standUp(double targetHeight);
void xMovement(double tX);
void hold();
void xTestMovement();
void pitchTestMovement();
// essential movements
void inverseKinematics(int legIndex, double torsoHeight, double x);
double cosineTheorem(int a, int b, double c);
//---

//--- esp driver test
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

int servoNumber = 0;
//---

void setup(){
  standDown();
  delay(1000);
}
void loop(){
  standUp(5);

  delay(1000);

  standDown();
}


// --- basic movements

void standUp(double targetHeight){
  double servoStep = 0.25;
  int delayTime = 50;
  
  while(height < targetHeight){ // assume all y's are equal
    height += servoStep;
    y[0] += servoStep;
    y[1] += servoStep;
    y[2] += servoStep;
    y[3] += servoStep;
    inverseKinematics(0, y[0], 0);
    inverseKinematics(1, y[1], 0);
    inverseKinematics(2, y[2], 0);
    inverseKinematics(3, y[3], 0);
    delay(delayTime);
  }
}

void standDown(){
  double servoStep = 0.25;
  int delayTime = 50;
  
  while(height > 0){// assume all y's are equal
    height -= servoStep;
    y[0] -= servoStep;
    y[1] -= servoStep;
    y[2] -= servoStep;
    y[3] -= servoStep;
    inverseKinematics(0, y[0], 0);
    inverseKinematics(1, y[1], 0);
    inverseKinematics(2, y[2], 0);
    inverseKinematics(3, y[3], 0);
    delay(delayTime);
  }
}
// --- essensials

void inverseKinematics(int legIndex, double torsoHeight, double x){
  
  double theta = atan( tan( x / torsoHeight ) ) * 57296 / 1000;
  double cNew = sqrt(torsoHeight*torsoHeight + x*x);

  if(cNew < 0){
    cNew *= -1;
  }
  
  int a = legLenght;
  int b = a;
  double c = cNew;

  int kneeServoIndex = leg[legIndex][0];
  int kneeMin = servoRoms[legIndex][0];
  int kneeAngle = cosineTheorem(a, b, c);
  int femurServoIndex = leg[legIndex][1];
  int femurMax = servoRoms[legIndex][3];
  int femurAngle = 90 - cosineTheorem(a, c, b);
  femurAngle += theta;

  //knee
  HCPCA9685.Servo(kneeServoIndex, map(kneeMin + kneeAngle, 0, 180, 1, 450));
  
  //femur
  HCPCA9685.Servo(femurServoIndex, map(femurMax - femurAngle, 0, 180, 1, 450));

  //hip
  int hipServoIndex = leg[legIndex][2];
  //the value bellow is meant to be a temporary patch
  //TODO: to set default hip servo pos to 90 degrees
  int hipMiddleAngle = servoRoms[legIndex][4];
  HCPCA9685.Servo(hipServoIndex, map(hipMiddleAngle, 0, 180, 1, 450));

}

double cosineTheorem(int a, int b, double c){
  if (c == 0){
    return 0;
  }
  else{
    return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
  }
}