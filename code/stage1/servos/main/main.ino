#include "HCPCA9685.h"

//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 170, 0, 0}, {3, 120, 85, 156, 0, 0}, {0, 115, 90, 150, 0, 0}, {0, 115, 100, 180, 0, 0}};
//the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

void setup() {
  //init the servo driver
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);

  //what i want:
  // - when the program starts move the body to lowered position
  // - stand up

  //make the robot to sit down
  inverseKinematics(0, 0);
  inverseKinematics(1, 0);
  inverseKinematics(2, 0);
  inverseKinematics(3, 0);
  
  // init pause
  delay(750);
}

void loop() {
  
  standUp();
  delay(3000);
  
  layDown();
  delay(3000);
  
}

void standUp(){
  
  int i = 0;

//  while(servoCurrentAngles[0][1] != servoRoms[0][3] - cosineTheorem(legLenght, legLenght, targetTorso)/2 ){
  while(i < 50){
    inverseKinematicsDelayed(0, 11);
    inverseKinematicsDelayed(1, 11);
    inverseKinematicsDelayed(2, 11);
    inverseKinematicsDelayed(3, 11);
    delay(50);
    i++;
  }
  
}

void layDown(){
  
  int i = 0;

//  while(servoCurrentAngles[0][1] != servoRoms[0][3] - cosineTheorem(legLenght, legLenght, targetTorso)/2 ){
  while(i < 50){
    inverseKinematicsDelayed(0, 0);
    inverseKinematicsDelayed(1, 0);
    inverseKinematicsDelayed(2, 01);
    inverseKinematicsDelayed(3, 0);
    delay(50);
    i++;
  }
  
}

void inverseKinematicsDelayed(int legIndex, double torsoHeight){
  // --- improved version of inverseKinematics() ---
  // provides smooth movement to the calculated target angle
  // !!! this function needs to be looped inside ' loop() '
  
  int a = legLenght; 
  int b = a;
  double c = torsoHeight;

  int kneeServoIndex = leg[legIndex][0]; 
  int kneeMin = servoRoms[legIndex][0]; 
  int kneeAngle = cosineTheorem(a, b, c)/2; 
  int femurServoIndex = leg[legIndex][1]; 
  int femurMin = servoRoms[legIndex][2];
  int femurMax = servoRoms[legIndex][3];
  int femurAngle = cosineTheorem(a, c, b)/2; 

  if (kneeMin + kneeAngle != servoCurrentAngles[legIndex][0]) {
    // check to see if we add to or remove from the current angle 
    // the direction check is inside the leg movement 'if' because we need to check ' kneeAngle != servoCurrentAngles[legIndex][0] '
    
    int kneeDirectionAngle;
    if(kneeMin + kneeAngle > servoCurrentAngles[legIndex][0]) { // 33
      kneeDirectionAngle = 1;
    }
    else{
      kneeDirectionAngle = -1;
    }
  
    // actual knee movement
    HCPCA9685.Servo(kneeServoIndex, map(servoCurrentAngles[legIndex][0] + kneeDirectionAngle, 0, 180, 1, 450));
    servoCurrentAngles[legIndex][0] += kneeDirectionAngle;
  }
  
  if (femurMax - femurAngle != servoCurrentAngles[legIndex][1]) {
    // check to see if we add to or remove from the current angle 
    // the direction check is inside the leg movement 'if' because we need to check ' femurAngle != servoCurrentAngles[legIndex][1] '
    int femurDirectionAngle;
    if(femurMax - kneeAngle > servoCurrentAngles[legIndex][0]) {
      femurDirectionAngle = 1;
    }
    else{
      femurDirectionAngle = -1;
    }
    
    // actual femur movement
    HCPCA9685.Servo(femurServoIndex, map(servoCurrentAngles[legIndex][1] - femurDirectionAngle, 0, 180, 1, 450));
    servoCurrentAngles[legIndex][1] -= femurDirectionAngle;
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
  int femurMax = servoRoms[legIndex][3];
  int femurAngle = cosineTheorem(a, c, b)/2;

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
