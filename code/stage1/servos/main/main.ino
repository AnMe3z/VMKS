#include "HCPCA9685.h"

//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 150, 0, 0}, {3, 120, 85, 135, 0, 0}, {0, 115, 90, 150, 0, 0}, {0, 115, 100, 160, 0, 0}};
//the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;
int height = 1;

void setup() {
  //init the servo driver
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);

  //what i want:
  // - when the program starts move the body to lowered position
  // - stand up

  //make the robot to sit down
  standDown();
  
  // init pause
  delay(750);
}

void loop() {

  standUp();

  standDown();
  
}

// --- movements

void standUp(){
  while(height < 15){
    height++;
    inverseKinematics(0, height);
    inverseKinematics(1, height);
    inverseKinematics(2, height);
    inverseKinematics(3, height);
    delay(300);
  }
}

void standDown(){
  while(height > 0){
    height--;
    inverseKinematics(0, height);
    inverseKinematics(1, height);
    inverseKinematics(2, height);
    inverseKinematics(3, height);
    delay(300);
  }
}

// --- essensials

void inverseKinematics(int legIndex, double torsoHeight){
  int a = legLenght;
  int b = a;
  double c = torsoHeight;

  int kneeServoIndex = leg[legIndex][0];
  int kneeMin = servoRoms[legIndex][0];
  int kneeAngle = cosineTheorem(a, b, c);
  int femurServoIndex = leg[legIndex][1];
  int femurMin = servoRoms[legIndex][2];
  int femurMax = servoRoms[legIndex][3];
  int femurAngle = 90 - cosineTheorem(a, c, b);

  //knee
  HCPCA9685.Servo(kneeServoIndex, map(kneeMin + kneeAngle, 0, 180, 1, 450));
  
  //femur
  HCPCA9685.Servo(femurServoIndex, map(femurMax - femurAngle, 0, 180, 1, 450));
}

double cosineTheorem(int a, int b, double c){
  if (c == 0){
    return 0;
  }
  else{
    return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
  }
}
