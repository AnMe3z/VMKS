#include<HCPCA9685.h>
#include<SoftwareSerial.h>
#include "NewPing.h"

// --- servo driver
#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

// --- legs
//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 150, 35, 0}, {3, 120, 85, 135, 75, 0}, {0, 115, 90, 150, 85, 0}, {0, 125,  90, 150, 80, 0}};
//the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

double x[4] = {0 ,0, 0, 0};
double height = 1;
double y[4] = {0 ,0, 0, 0};
double z[4] = {0, 0, 0, 0};

//all functions predeclaration
// complex movements
void walk(int speed);
void reset();
// basic movements
void standDown();
void standUp(double targetHeight);
void pitch(double angle);
// essential movements
void inverseKinematicsZTest(int legIndex, double torsoHeight, double x);
void inverseKinematics(int legIndex, double torsoHeight, double x);
double cosineTheorem(int a, int b, double c);

void setup() {
  //init the servo driver
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);
  
  Serial.begin(9600); /* Define baud rate for serial communication */

  standDown();
  standUp(5);

  // init pause
  delay(750);
  
}

void loop() {
  
  // walk(50);
  // reset();

  pitch(10);

  delay(1000);

  walk(50);
  reset();

  delay(1000);
  
  pitch(-10);
  
  delay(10000);

}

// --- complex movements

void reset(){
  y[0] = 5;
  x[0] = 0;
  y[1] = 5;
  x[1] = 0;
  y[2] = 5;
  x[2] = 0;
  y[3] = 5;
  x[3] = 0;
  inverseKinematics(0, y[0], x[0]);
  inverseKinematics(1, y[1], x[1]);
  inverseKinematics(2, y[2], x[2]);
  inverseKinematics(3, y[3], x[3]);
  delay(150);
}

void walk(int speed){
  int archSteps = 10;
  double angle = 180;
  double radius = 1;

  //legs upward drift
  int count = 0;
  bool rising = true;
  double lUDStep = radius/2;

  //legs drift step
  double lDStep = (2 * radius) / (angle / archSteps);

  //pitch
  pitch(25);
  delay(1000);

  double cx[4] = {x[0] + radius/2, x[1] + radius/2, x[2] + radius/2, x[3] + radius/2};
  double cy[4] = {y[0], y[1], y[2], y[3]};

  while(angle > 0){
    angle -= 180/archSteps;

    x[1] = cx[1] + radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematics(1, y[1], x[1]);

    x[2] = cx[2] + radius * cos((angle * 71) / 4068);
    y[2] = cy[2] - radius * sin((angle * 71) / 4068);
    inverseKinematics(2, y[2], x[2]);

    //leg drift
    if(count < 6 && rising){
      count++;
      if(count == 6){
        rising = false;
      }
    }

    if(rising){
      y[0] += lUDStep*0.75;
      y[3] += lUDStep*0.75;
    }
    else{
      y[0] -= lUDStep*0.75;
      y[3] -= lUDStep*0.75;
    }

    x[0] -= lDStep/2;
    inverseKinematics(0, y[0], x[0]);
    x[3] -= lDStep/2;
    inverseKinematics(3, y[3], x[3]);

    delay(speed);
  }

  // reset the angle
  angle = 180;
  count = 0;
  rising = true;
  y[1] = height;
  y[2] = height;
  double bx[4] = {x[0] + radius, x[1] + radius, x[2] + radius, x[3] + radius};
  double by[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cx, bx, sizeof(cx));
  memcpy(cy, by, sizeof(cy));

  while(angle > 0){ // loops 10 times
    angle -= 180/archSteps;

    x[0] = cx[0] + radius * cos((angle * 71) / 4068);
    y[0] = cy[0] - (radius * 1.4) * sin((angle * 71) / 4068);
    inverseKinematics(0, y[0], x[0]);

    x[3] = cx[3] + radius * cos((angle * 71) / 4068);
    y[3] = cy[3] - (radius * 1.4) * sin((angle * 71) / 4068);
    inverseKinematics(3, y[3], x[3]);

    //leg drift
    if(count < 6 && rising){
      count++;
      if(count == 6){
        rising = false;
      }
    }

    if(rising){
      y[1] += lUDStep;
      y[2] += lUDStep;
    }
    else{
      y[1] -= lUDStep;
      y[2] -= lUDStep;
    }

    x[1] -= lDStep;
    inverseKinematics(1, y[1], x[1]);
    x[2] -= lDStep;
    inverseKinematics(2, y[2], x[2]);

    delay(speed);
  }

  // // reset variables
  angle = 180;
  y[0] = height;
  y[3] = height;
  double ax[4] = {x[0] + radius/2, x[1] + radius/2, x[2] + radius/2, x[3] + radius/2};
  double ay[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cx, ax, sizeof(cx));
  memcpy(cy, ay, sizeof(cy));

  while(angle > 180){
    angle -= 180/archSteps;

    x[1] = cx[1] + radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematics(1, y[1], x[1]);

    x[2] = cx[2] + radius * cos((angle * 71) / 4068);
    y[2] = cy[2] - radius * sin((angle * 71) / 4068);
    inverseKinematics(2, y[2], x[2]);


    //leg drift
    if(count < 6 && rising){
      count++;
      if(count == 6){
        rising = false;
      }
    }

    if(rising){
      y[0] += lUDStep*0.75;
      y[3] += lUDStep*0.75;
    }
    else{
      y[0] -= lUDStep*0.75;
      y[3] -= lUDStep*0.75;
    }

    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(3, y[3], x[3]);

    delay(speed);
  }
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

void pitch(double angle){
  
  double bodyLenght = 33.00;
  double radius = bodyLenght/2;

  double pitchOffsetX = radius - cos((angle * 71) / 4068)*radius;

  x[0] += pitchOffsetX;
  x[1] += pitchOffsetX;
  x[2] -= pitchOffsetX;
  x[3] -= pitchOffsetX;

  y[0] += pitchOffsetX;
  y[1] += pitchOffsetX;
  y[2] -= pitchOffsetX;
  y[3] -= pitchOffsetX;

  inverseKinematics(0, y[0], x[0]);
  inverseKinematics(1, y[1], x[1]);
  inverseKinematics(2, y[2], x[2]);
  inverseKinematics(3, y[3], x[3]);
}

// --- essensials

void inverseKinematics(int legIndex, double torsoHeight, double x){
  //
  if(legIndex == 2){
    if(height == 0){torsoHeight -= 1;}
    else{torsoHeight += 1;}
  }

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