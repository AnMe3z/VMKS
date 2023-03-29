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
double height = 1;
double x = 0;
double pitch[4] = {0 ,0, 0, 0};

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
  
  standUp(7);

  pitchTestMovement();

  xTestMovement();
  
  hold();
  delay(3000);

  standDown();
  
}

// --- movements

void standUp(double targetHeight){
  double servoStep = 0.25;
  int delayTime = 50;
  
  while(height < targetHeight){
    height += servoStep;
    inverseKinematics(0, height, 0);
    inverseKinematics(1, height, 0);
    inverseKinematics(2, height, 0);
    inverseKinematics(3, height, 0);
    delay(delayTime);
  }
}

void standDown(){
  double servoStep = 0.25;
  int delayTime = 50;
  
  while(height > 0){
    height -= servoStep;
    inverseKinematics(0, height, 0);
    inverseKinematics(1, height, 0);
    inverseKinematics(2, height, 0);
    inverseKinematics(3, height, 0);
    delay(delayTime);
  }
}

void hold(){
  int delayTime = 50;
  
  inverseKinematics(0, height - pitch[0], 0);
  inverseKinematics(1, height - pitch[1], 0);
  inverseKinematics(2, height - pitch[2], 0);
  inverseKinematics(3, height - pitch[3], 0);

  delay(delayTime);
}

void xTestMovement(){
  double servoStep = 0.25;
  int delayTime = 50;
  
  while(x > -2){
    x -= servoStep;
    inverseKinematics(0, height, x);
    inverseKinematics(1, height, x);
    inverseKinematics(2, height, x);
    inverseKinematics(3, height, x);
    delay(delayTime);
  }
  
  while(x < 2){
    x += servoStep;
    inverseKinematics(0, height, x);
    inverseKinematics(1, height, x);
    inverseKinematics(2, height, x);
    inverseKinematics(3, height, x);
    delay(delayTime);
  }
  
  while(x > 0){
    x -= servoStep;
    inverseKinematics(0, height, x);
    inverseKinematics(1, height, x);
    inverseKinematics(2, height, x);
    inverseKinematics(3, height, x);
    delay(delayTime);
  }

  delay(1000);
}

void pitchTestMovement(){
  double servoStep = 0.25;
  int delayTime = 50;
  int i = 0;
  
  while(i < 12){
    i++;
    pitch[0] += servoStep;
    pitch[1] += servoStep*2;
    pitch[2] += 0;
    pitch[3] += servoStep;
    inverseKinematics(0, height - pitch[0], 0);
    inverseKinematics(1, height - pitch[1], 0);
    inverseKinematics(2, height - pitch[2], 0);
    inverseKinematics(3, height - pitch[3], 0);
    delay(delayTime);
  }
  while(i > 0){
    i--;
    pitch[0] -= servoStep;
    pitch[1] -= servoStep;
    pitch[2] += servoStep;
    pitch[3] += servoStep;
    inverseKinematics(0, height - pitch[0], 0);
    inverseKinematics(1, height - pitch[1], 0);
    inverseKinematics(2, height - pitch[2], 0);
    inverseKinematics(3, height - pitch[3], 0);
    delay(delayTime);
  }
  while(i < 12){
    i++;
    pitch[0] += servoStep;
    pitch[1] -= servoStep;
    pitch[2] += servoStep;
    pitch[3] -= servoStep;
    inverseKinematics(0, height - pitch[0], 0);
    inverseKinematics(1, height - pitch[1], 0);
    inverseKinematics(2, height - pitch[2], 0);
    inverseKinematics(3, height - pitch[3], 0);
    delay(delayTime);
  }
  while(i > 0){
    i--;
    pitch[0] += servoStep;
    pitch[1] += servoStep;
    pitch[2] -= servoStep;
    pitch[3] -= servoStep;
    inverseKinematics(0, height - pitch[0], 0);
    inverseKinematics(1, height - pitch[1], 0);
    inverseKinematics(2, height - pitch[2], 0);
    inverseKinematics(3, height - pitch[3], 0);
    delay(delayTime);
  }
  while(i < 12){
    i++;
    pitch[0] -= servoStep*2;
    pitch[1] -= servoStep;
    pitch[2] -= servoStep;
    pitch[3] += 0;
    inverseKinematics(0, height - pitch[0], 0);
    inverseKinematics(1, height - pitch[1], 0);
    inverseKinematics(2, height - pitch[2], 0);
    inverseKinematics(3, height - pitch[3], 0);
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
  int femurMin = servoRoms[legIndex][2];
  int femurMax = servoRoms[legIndex][3];
  int femurAngle = 90 - cosineTheorem(a, c, b);
  femurAngle += theta;

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
