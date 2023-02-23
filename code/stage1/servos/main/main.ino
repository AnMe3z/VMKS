#include "HCPCA9685.h"
//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

int servooms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

// kak bi rabotil mapinga 
// Servo.write(map(angle, 0, 180, servo-roms[0][0], servo-roms[0][1]));

int legLenght = 10;

void setup() {
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);
}

void loop() {
  inverseKinematics(0, 6);
  delay(500);
  inverseKinematics(0, 7);
  delay(500);
  inverseKinematics(0, 8);
  delay(500);
  inverseKinematics(0, 9);
  delay(500);
  inverseKinematics(0, 10);
  delay(500);
  inverseKinematics(0, 11);
  delay(500);
  inverseKinematics(0, 12);
  delay(500);
  inverseKinematics(0, 13);
  delay(500);
  inverseKinematics(0, 14);
  delay(500);
  inverseKinematics(0, 15);
  delay(500);
  inverseKinematics(0, 16);
  delay(500);
  inverseKinematics(0, 15);
  delay(500);
  inverseKinematics(0, 14);
  delay(500);
  inverseKinematics(0, 13);
  delay(500);
  inverseKinematics(0, 12);
  delay(500);
  inverseKinematics(0, 11);
  delay(500);
  inverseKinematics(0, 10);
  delay(500)
  inverseKinematics(0, 9);
  delay(500);
}

void inverseKinematics(int legIndex, int torsoHeight){
  int a = legLenght;
  int b = a;
  int c = torsoHeight;

  int kneeServoIndex = 0;//leg[legIndex][legIndex*3];
  int kneeAngle = cosineTheorem(a, b, c)/2;
  int femurServoIndex = 1;//leg[legIndex][legIndex*3 + 1];
  int femurAngle = cosineTheorem(a, c, b);
  
  //knee
  HCPCA9685.Servo(kneeServoIndex, map(72 + kneeAngle, 0, 180, 10, 450));
  //femur
  HCPCA9685.Servo(femurServoIndex, map(74 + femurAngle, 0, 180, 10, 450));
}

int cosineTheorem(int a, int b, int c){
  return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
}
