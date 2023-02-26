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

// kak bi rabotil mapinga 
// Servo.write(map(angle, 0, 180, servo-roms[0][0], servo-roms[0][1]));

int legLenght = 10;

double torso = 2.0;
bool bodyUp;

void setup() {
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);

  inverseKinematics(0, 2);
  inverseKinematics(1, 2);
  inverseKinematics(2, 2);
  inverseKinematics(3, 2);
  bodyUp = true;
  
  delay(3000);
}

void loop() {
  
//  inverseKinematics(0, 2);
//  inverseKinematics(1, 2);
//  inverseKinematics(2, 2);
//  inverseKinematics(3, 2);
//  delay(3000);
//  inverseKinematics(0, 12);
//  inverseKinematics(1, 12);
//  inverseKinematics(2, 12);
//  inverseKinematics(3, 12);
//  delay(3000);
    
  // --- up and down loop code
  if(bodyUp){
    torso += 0.2;
    if(torso == 12.0){
      bodyUp = false;
    }
  }
  else{
    torso -= 0.2;
    if(torso == 2.0){
      bodyUp = true;
    }
  }
  
  inverseKinematics(0, torso);
  inverseKinematics(1, torso);
  inverseKinematics(2, torso);
  inverseKinematics(3, torso);
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
  int femurAngle = cosineTheorem(a, c, b);
  
  //knee
  HCPCA9685.Servo(kneeServoIndex, map(kneeMin + kneeAngle, 0, 180, 1, 450));
  //femur
  HCPCA9685.Servo(femurServoIndex, map(femurMin + femurAngle, 0, 180, 1, 450));
}

double cosineTheorem(int a, int b, double c){
  return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
}
