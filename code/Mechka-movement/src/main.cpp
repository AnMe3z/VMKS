#include "HCPCA9685.h"

//must have:
// - array with min and max values
// - inverse kinematics

#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 150, 35, 0}, {3, 120, 85, 135, 75, 0}, {0, 115, 90, 150, 85, 0}, {0, 115, 100, 160, 90, 0}};
//the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

double x[4] = {0 ,0, 0, 0};
double height = 1;
double y[4] = {0 ,0, 0, 0};

//all functions predeclaration
// complex movements
void walk(int speed);
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
  
  standUp(5);

  // walk(50);
  // y[0] = 5;
  // x[0] = 0;
  // y[1] = 5;
  // x[1] = 0;
  // y[2] = 5;
  // x[2] = 0;
  // y[3] = 5;
  // x[3] = 0;
  // inverseKinematics(0, y[0], x[0]);
  // inverseKinematics(1, y[1], x[1]);
  // inverseKinematics(2, y[2], x[2]);
  // inverseKinematics(3, y[3], x[3]);
  // delay(150);

  // walk(50);

  // y[0] = 5;
  // x[0] = 0;
  // y[1] = 5;
  // x[1] = 0;
  // y[2] = 5;
  // x[2] = 0;
  // y[3] = 5;
  // x[3] = 0;
  // inverseKinematics(0, y[0], x[0]);
  // inverseKinematics(1, y[1], x[1]);
  // inverseKinematics(2, y[2], x[2]);
  // inverseKinematics(3, y[3], x[3]);
  // delay(150);
  
  // walk(50);
  
  // delay(1000);

  // standDown();
  
}

// --- complex movements

void walk(int speed){
  int archSteps = 10;
  double angle = 180;
  double radius = 1.0;

  //legs upward drift
  // int count = 0;
  // bool rising = true;
  // double lUDStep = (radius) / (angle/archSteps);

  //legs drift step
  double lDStep = (4 * radius) / (angle / archSteps);

  //pitch 
  y[2] -= 0.5;
  y[3] -= 0.5;
  inverseKinematics(2, y[2], x[2]);
  inverseKinematics(3, y[3], x[3]);
  delay(speed);
  
  double cx[4] = {x[0] + radius, x[1] + radius, x[2] + radius, x[3] + radius};
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
    x[0] -= lDStep/2;
    inverseKinematics(0, y[0], x[0]);
    x[3] -= lDStep/2;
    inverseKinematics(3, y[3], x[3]);

    delay(speed);
  }

  // reset the angle
  angle = 180;
  double bx[4] = {x[0] + radius, x[1] + radius, x[2] + radius, x[3] + radius};
  double by[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cx, bx, sizeof(cx));
  memcpy(cy, by, sizeof(cy));
  y[1] = height;
  y[2] = height;
  
  while(angle > 0){ // loops 18 times
      angle -= 180/archSteps;

      x[0] = cx[0] + radius * cos((angle * 71) / 4068);
      y[0] = cy[0] - radius * sin((angle * 71) / 4068);
      inverseKinematics(0, y[0], x[0]);

      x[3] = cx[3] + radius * cos((angle * 71) / 4068);
      y[3] = cy[3] - radius * sin((angle * 71) / 4068);
      inverseKinematics(3, y[3], x[3]);

      //leg drift
      x[1] -= lDStep;
      inverseKinematics(1, y[1], x[1]);
      x[2] -= lDStep;
      inverseKinematics(2, y[2], x[2]);

    delay(speed);
  }
  
  // // reset variables
  // angle = 180;
  // double ax[4] = {x[0] + radius, x[1] + radius, x[2] + radius, x[3] + radius};
  // double ay[4] = {y[0], y[1], y[2], y[3]};
  // memcpy(cx, ax, sizeof(cx));
  // memcpy(cy, ay, sizeof(cy));
  // y[0] = height;
  // y[3] = height;

  // while(angle > 90){
  //   angle -= 180/archSteps;

  //   x[1] = cx[1] + radius * cos((angle * 71) / 4068);
  //   y[1] = cy[1] - radius * sin((angle * 71) / 4068);
  //   inverseKinematics(1, y[1], x[1]);

  //   x[2] = cx[2] + radius * cos((angle * 71) / 4068);
  //   y[2] = cy[2] - radius * sin((angle * 71) / 4068);
  //   inverseKinematics(2, y[2], x[2]);

  //   //leg drift
  //   x[0] -= lDStep;
  //   inverseKinematics(0, y[0], x[0]);
  //   x[3] -= lDStep;
  //   inverseKinematics(3, y[3], x[3]);

  //   delay(150);
  // }
  
  
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

void xMovement(double tX){
  double cX = x[0]; // current x
  double servoStep = 0.25;
  int delayTime = 50;
  
  if(cX < tX){
    while(x[0] < tX){
      cX += servoStep;
      x[0] += servoStep;
      x[1] += servoStep;
      x[2] += servoStep;
      x[3] += servoStep;
      inverseKinematics(0, y[0], x[0]);
      inverseKinematics(1, y[1], x[1]);
      inverseKinematics(2, y[2], x[2]);
      inverseKinematics(3, y[3], x[3]);
      delay(delayTime);
    }
  }
  else{
    while(cX > tX){
      cX -= servoStep;
      x[0] -= servoStep;
      x[1] -= servoStep;
      x[2] -= servoStep;
      x[3] -= servoStep;
      inverseKinematics(0, y[0], x[0]);
      inverseKinematics(1, y[1], x[1]);
      inverseKinematics(2, y[2], x[2]);
      inverseKinematics(3, y[3], x[3]);
      delay(delayTime);
    }
  }

  delay(1000);
}

void hold(){
  int delayTime = 50;
  
  inverseKinematics(0, height, 0);
  inverseKinematics(1, height, 0);
  inverseKinematics(2, height, 0);
  inverseKinematics(3, height, 0);

  delay(delayTime);
}

void xTestMovement(){
  double servoStep = 0.25;
  int delayTime = 50;
  double tX = x[0];
  
  while(tX > -2){
    tX -= servoStep;
    x[0] -= servoStep;
    x[1] -= servoStep;
    x[2] -= servoStep;
    x[3] -= servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  
  while(tX < 2){
    tX += servoStep;
    x[0] += servoStep;
    x[1] += servoStep;
    x[2] += servoStep;
    x[3] += servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  
  while(tX > 0){
    tX -= servoStep;
    x[0] -= servoStep;
    x[1] -= servoStep;
    x[2] -= servoStep;
    x[3] -= servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
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
    y[0] += servoStep;
    y[1] += servoStep*2;
    y[2] += 0;
    y[3] += servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  while(i > 0){
    i--;
    y[0] -= servoStep;
    y[1] -= servoStep;
    y[2] += servoStep;
    y[3] += servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  while(i < 12){
    i++;
    y[0] += servoStep;
    y[1] -= servoStep;
    y[2] += servoStep;
    y[3] -= servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  while(i > 0){
    i--;
    y[0] += servoStep;
    y[1] += servoStep;
    y[2] -= servoStep;
    y[3] -= servoStep;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
    delay(delayTime);
  }
  while(i < 12){
    i++;
    y[0] -= servoStep*2;
    y[1] -= servoStep;
    y[2] -= servoStep;
    y[3] += 0;
    inverseKinematics(0, y[0], x[0]);
    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);
    inverseKinematics(3, y[3], x[3]);
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