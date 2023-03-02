#include "HCPCA9685.h"

//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 170, 0, 0}, {3, 120, 85, 156, 0, 0}, {0, 115, 90, 150, 0, 0}, {0, 115, 100, 180, 0, 0}};
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

//double startAngle;

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

//  inverseKinematicsDelayed(0, 0);
//  inverseKinematicsDelayed(1, 0);
//  inverseKinematicsDelayed(2, 0);
//  inverseKinematicsDelayed(3, 0);
//  delay(3000);

  inverseKinematicsDelayed(0, 12);
  inverseKinematicsDelayed(1, 12);
  inverseKinematicsDelayed(2, 11);
  inverseKinematicsDelayed(3, 11);
  delay(500);
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
  int femurAngle = cosineTheorem(a, c, b);

  if (kneeAngle != servoCurrentAngles[legIndex][0]) {
    // check to see if we add to or remove from the current angle 
    // the direction check is inside the leg movement 'if' because we need to check ' kneeAngle != servoCurrentAngles[legIndex][0] '

    // !!! here we need to exclude leg 0 (aka down right leg)
    // this is needed beacuse the leg is the first one build with a lot of engeneering errors
    // therefore its closing knee angle is 40 instead of way lower angle like the others
    // this causes problems in lower torso hights and prevent the leg from moving at all
    // --- the code bellow is not optimized and it has to be deleted when the leg 0 is physically fixed
    if (legIndex == 0 && kneeAngle < 45) {
      int kneeDirectionAngle;
      kneeDirectionAngle = -1;
    }
    else{
      
      // the actual code to be left when leg 0 is fixed
      int kneeDirectionAngle;
      if(kneeAngle > servoCurrentAngles[legIndex][0]) {
        kneeDirectionAngle = 1;
      }
      else{
        kneeDirectionAngle = -1;
      }
      
    }
    int kneeDirectionAngle;
    if(kneeAngle > servoCurrentAngles[legIndex][0]) {
      kneeDirectionAngle = 1;
    }
    else{
      kneeDirectionAngle = -1;
    }
  
    // actual knee movement
    HCPCA9685.Servo(kneeServoIndex, map(servoCurrentAngles[legIndex][0] + kneeDirectionAngle, 0, 180, 1, 450));
    servoCurrentAngles[legIndex][0] += kneeDirectionAngle;
  }
  
  if (femurAngle != servoCurrentAngles[legIndex][1]) {
    // check to see if we add to or remove from the current angle 
    // the direction check is inside the leg movement 'if' because we need to check ' femurAngle != servoCurrentAngles[legIndex][1] '
    int femurDirectionAngle;
    if(kneeAngle > servoCurrentAngles[legIndex][0]) {
      femurDirectionAngle = 1;
    }
    else{
      femurDirectionAngle = -1;
    }
    
    // actual femur movement
    HCPCA9685.Servo(femurServoIndex, map(servoCurrentAngles[legIndex][1] + femurDirectionAngle, 0, 180, 1, 450));
    servoCurrentAngles[legIndex][1] += femurDirectionAngle;
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
