#include "HCPCA9685.h"

//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 170, 0, 0}, {3, 120, 85, 156, 0, 0}, {0, 115, 90, 150, 0, 0}, {0, 115, 100, 180, 0, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

double targetAngle; // real targeted angle: maxAngle - targetAngle
bool torsoMoving;

void setup() {
  //init the servo driver
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);

  //what i want:
  // - when the program starts move the body to lowered position
  // - stand up

  targetAngle = 1;

  // init pause
  delay(3000);
}

void loop() {

  inverseKinematicsDelayed(0, 0);
  inverseKinematicsDelayed(1, 0);
  inverseKinematicsDelayed(2, 0);
  inverseKinematicsDelayed(3, 0);
  delay(3000);

  inverseKinematicsDelayed(0, 12);
  inverseKinematicsDelayed(1, 12);
  inverseKinematicsDelayed(2, 11);
  inverseKinematicsDelayed(3, 11);
  delay(3000);
}

void inverseKinematicsDelayed(int legIndex, double torsoHeight){
  // --- improved version of inverseKinematics() ---
  // provides smooth movement to the calculated target angle
  
  int a = legLenght;
  int b = a;
  double c = torsoHeight;

  int kneeServoIndex = leg[legIndex][0];
  int kneeMin = servoRoms[legIndex][0];
  int kneeAngle = cosineTheorem(a, b, c)/2;
  int femurServoIndex = leg[legIndex][1];
  int femurMin = servoRoms[legIndex][2];
  int femurAngle = cosineTheorem(a, c, b);

  //knee
  for (int i = kneeMin; i <= kneeAngle; i++) {
    HCPCA9685.Servo(kneeServoIndex, map(i, 0, 180, 1, 450));
    delay(50);
  }
  
  //femur
  for (int i = femurMin; i <= femurAngle; i++) {
    HCPCA9685.Servo(femurServoIndex, map(i, 0, 180, 1, 450));
    delay(50);
  }
}

void inverseKinematics(int legIndex, double torsoHeight){
  int a = legLenght;
  int b = a;
  double c = torsoHeight;

  int kneeServoIndex = leg[legIndex][0];
  int kneeMin = servoRoms[legIndex][0];
  int kneeAngle = cosineTheorem(a, b, c)/2;
  int femurServoIndex = leg[legIndex][1];
  int femurMin = servoRoms[legIndex][2];
  int femurAngle = cosineTheorem(a, c, b);

  //knee
  HCPCA9685.Servo(kneeServoIndex, map(kneeMin + kneeAngle, 0, 180, 1, 450));
  
  //femur
  HCPCA9685.Servo(femurServoIndex, map(femurMin + femurAngle, 0, 180, 1, 450));
}

double cosineTheorem(int a, int b, double c){
  if (c == 0){
    return 0;
  }
  else{
    return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
  }
}
