#include<HCPCA9685.h>
#include<SoftwareSerial.h>
#include "NewPing.h"

// --- servo driver
#define  I2CAdd 0x40
HCPCA9685 HCPCA9685(I2CAdd);

// --- bluetooth
char receivedValueBT;

/* Create object named bt of the class SoftwareSerial */ 
SoftwareSerial bt(10,11); /* (Rx,Tx) */  

// --- ultrasonic sensors
#define TRIGGER_PIN 5
#define ECHO_PIN 6
#define TRIGGER_PIN2 9
#define ECHO_PIN2 4

#define MAX_DISTANCE 400 

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);

// --- legs
//int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
//                      c             c           c            c           c           c           c             c
int servoRoms[4][6] = {{40, 180, 90, 150, 35, 0}, {3, 120, 85, 135, 75, 0}, {0, 115, 90, 150, 85, 0}, {0, 125,  90, 150, 90, 0}};
//the legs need to be closed at the start of the program for the angles to be true
int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

int legLenght = 10;

double x[4] = {0 ,0, 0, 0};
double height = 1;
double y[4] = {0 ,0, 0, 0};
double z[4] = {0, 0, 0, 0};

//all functions predeclaration
//sensors
bool checkDistance(); // returns true if there is NO obstacle
// complex movements
void rotateRight(int speed);
void workout(int reps);
void sideStep();
void walk(int speed);
void reset();
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
void inverseKinematicsZTest(int legIndex, double torsoHeight, double x);
void inverseKinematics(int legIndex, double torsoHeight, double x);
double cosineTheorem(int a, int b, double c);

void setup() {
  //init the servo driver
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);
  bt.begin(9600); /* Define baud rate for software serial communication */
  Serial.begin(9600); /* Define baud rate for serial communication */

  standDown();
  // standUp(5);

  // init pause
  delay(750);
  
}

void loop() {
  if (!checkDistance()){
    standUp(5);
  }

  if (bt.available()) 
    {
      receivedValueBT = bt.read();
      if (receivedValueBT == '1') // walk
        {
          Serial.write("One");
          walk(50);
          reset();
        }
      else if (receivedValueBT == '2') // left
        {
          Serial.write("Two");
          sideStep();
        }
      else if (receivedValueBT == '3') // right
        {
          Serial.write("Three");
          rotateRight(50);
        }
      else if (receivedValueBT == '4') // reverse
        {
          Serial.write("Four");
          walkReverse(50);
        }
      else if (receivedValueBT == '5') // standUp
        {
          Serial.write("Five");
          standUp(5);
        }
      else if (receivedValueBT == '6') // standDown
        {
          Serial.write("Six");
          standDown();
        }
      else if (receivedValueBT == '7') // xTest
        {
          Serial.write("Seven");
          xTestMovement();
        }
      else if (receivedValueBT == '8') // pitchTest
        {
          Serial.write("Eight");
          pitchTestMovement();
        }
      else if (receivedValueBT == '9') // 180 degrees - work in progress
        {
          Serial.write("Nine");
          pitchTestMovement();
        }
      else if (receivedValueBT == 'A') // SidestepLEFT
        {
          Serial.write("Ten - A");
          pitchTestMovement();
        }
      else if (receivedValueBT == 'B') // SidestepRIGHT
        {
          Serial.write("Eleven - B");
          pitchTestMovement();
        }
      else if (receivedValueBT == 'C') //pushups
        {
          Serial.write("Twelve - C");
          workout(5);
        }
    }
  else{
  }
  delay(100);

}

// --- sensors
bool checkDistance(){
  if(sonar.ping_cm() >= 7){
    return true;
  }
  else{
    return false;
  }
}

// --- complex movements

void rotateRight(int speed){

  int archSteps = 10;
  double angle = 180;
  double radius = 1;

  //legs upward drift
  int count = 0;
  bool rising = true;
  double lUDStep = radius/2;

  //legs drift step
  double lDStep = (2 * radius) / (angle / archSteps);

  double cx[4] = {x[0] + radius/2, x[1] + radius/2, x[2] + radius/2, x[3] + radius/2};
  double cy[4] = {y[0], y[1], y[2], y[3]};

  //leg lift towards rotating direction
  double targetLift = 3;
  double liftStep = targetLift/archSteps;

  while(angle > 0){
    angle -= 180/archSteps;

    x[1] = cx[1] + radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematics(1, y[1], x[1]);

    //lift rotation direction leg
    y[2] += liftStep;
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
  // y[2] = height;
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
      // y[2] += lUDStep;
    }
    else{
      y[1] -= lUDStep;
      // y[2] -= lUDStep;
    }

    x[1] -= lDStep;
    inverseKinematics(1, y[1], x[1]);
    // x[2] -= lDStep;
    // inverseKinematics(2, y[2], x[2]);

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

    //land rotation direction leg
    y[2] -= liftStep;
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
  
  //experimental
  // height = 0;
}

void workout(int reps){
  double targetHeight = 7.0;
  double servoStep = 0.25;
  int delayTime = 50;

  standDown();

  double pushupHeight = 0;

  //push up movement
  for (int i = 0; i <= reps; i++) {
    // pushing up

    while(pushupHeight <= targetHeight){ // assume all y's are equal
      pushupHeight += servoStep;
      y[2] += servoStep*1.25;
      y[3] += servoStep;
      inverseKinematics(2, y[2], 0);
      inverseKinematics(3, y[3], 0);
      delay(delayTime);
    }

    //standing down
    while(pushupHeight > 0){// assume all y's are equal
        pushupHeight -= servoStep;
        y[2] -= servoStep*1.25;
        y[3] -= servoStep;
        inverseKinematics(2, y[2], 0);
        inverseKinematics(3, y[3], 0);
        delay(delayTime);
      }
  }
}

void sideStep(){
  
  int archSteps = 10;
  double angle = 180;
  double radius = 0.5;

  //legs upward drift
  int count = 0;
  bool rising = true;
  double lUDStep = radius/2;

  //legs drift step
  double lDStep = (2 * radius) / (angle / archSteps);

  double cz[4] = {z[0] + radius/2, z[1] + radius/2, z[2] + radius/2, z[3] + radius/2};
  double cy[4] = {y[0], y[1], y[2], y[3]};

  while(angle > 0){
    angle -= 180/archSteps;

    z[1] = cz[1] + radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematicsZTest(1, y[1], z[1]);

    z[2] = cz[2] + radius * cos((angle * 71) / 4068);
    y[2] = cy[2] - radius * sin((angle * 71) / 4068);
    inverseKinematicsZTest(2, y[2], z[2]);

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

    z[0] -= lDStep/2;
    inverseKinematicsZTest(0, y[0], z[0]);
    z[3] -= lDStep/2;
    inverseKinematicsZTest(3, y[3], z[3]);

    delay(50);
  }

  // reset the angle
  angle = 180;
  count = 0;
  rising = true;
  y[1] = height;
  y[2] = height;
  double bz[4] = {z[0] + radius, z[1] + radius, z[2] + radius, z[3] + radius};
  double by[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cz, bz, sizeof(cz));
  memcpy(cy, by, sizeof(cy));

  while(angle > 0){ // loops 10 times
    angle -= 180/archSteps;

    z[0] = cz[0] + radius * cos((angle * 71) / 4068);
    y[0] = cy[0] - (radius * 1.4) * sin((angle * 71) / 4068);
    inverseKinematicsZTest(0, y[0], z[0]);

    z[3] = cz[3] + radius * cos((angle * 71) / 4068);
    y[3] = cy[3] - (radius * 1.4) * sin((angle * 71) / 4068);
    inverseKinematicsZTest(3, y[3], z[3]);

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

    z[1] -= lDStep;
    inverseKinematicsZTest(1, y[1], z[1]);
    z[2] -= lDStep;
    inverseKinematicsZTest(2, y[2], z[2]);

    delay(50);
  }

  // // reset variables
  angle = 180;
  y[0] = height;
  y[3] = height;
  double az[4] = {z[0] + radius/2, z[1] + radius/2, z[2] + radius/2, z[3] + radius/2};
  double ay[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cz, az, sizeof(cz));
  memcpy(cy, ay, sizeof(cy));

  while(angle > 180){
    angle -= 180/archSteps;

    z[1] = cz[1] + radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematicsZTest(1, y[1], z[1]);

    z[2] = cz[2] + radius * cos((angle * 71) / 4068);
    y[2] = cy[2] - radius * sin((angle * 71) / 4068);
    inverseKinematicsZTest(2, y[2], z[2]);


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

    inverseKinematicsZTest(0, y[0], z[0]);
    inverseKinematicsZTest(3, y[3], z[3]);

    delay(50);
  }
}

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

void walkTrust(int speed){
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
  // y[2] -= 1;
  // y[3] -= 1;
  // inverseKinematics(2, y[2], x[2]);
  // inverseKinematics(3, y[3], x[3]);
  // delay(speed);

  double cx[4] = {x[0] + radius/2, x[1] + radius/2, x[2] + radius/2, x[3] + radius/2};
  double cy[4] = {y[0], y[1], y[2], y[3]};

  while(angle > 0){
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

    x[0] = cx[0] + radius * cos((angle * 71) / 4068);
    y[0] = cy[0] - radius * sin((angle * 71) / 4068);
    inverseKinematics(0, y[0], x[0]);

    x[3] = cx[3] + radius * cos((angle * 71) / 4068);
    y[3] = cy[3] - radius * sin((angle * 71) / 4068);
    inverseKinematics(3, y[3], x[3]);


    //leg drift
    if(count < 6 && rising){
      count++;
      if(count == 6){
        rising = false;
      }
    }

    if(rising){
      y[1] += lUDStep*0.75;
      y[2] += lUDStep*0.75;
    }
    else{
      y[1] -= lUDStep*0.75;
      y[2] -= lUDStep*0.75;
    }

    inverseKinematics(1, y[1], x[1]);
    inverseKinematics(2, y[2], x[2]);

    delay(150);
  }


}

void walkReverse(int speed){
int archSteps = 10;
  double angle = 180;
  double radius = 1;

  //legs upward drift
  int count = 0;
  bool rising = true;
  double lUDStep = radius/2;

  //legs drift step
  double lDStep = (2 * radius) / (angle / archSteps);

  double cx[4] = {x[0] - radius/2, x[1] - radius/2, x[2] - radius/2, x[3] - radius/2};
  double cy[4] = {y[0], y[1], y[2], y[3]};

  while(angle > 0){
    angle -= 180/archSteps;

    x[1] = cx[1] - radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematics(1, y[1], x[1]);

    x[2] = cx[2] - radius * cos((angle * 71) / 4068);
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

    x[0] += lDStep/2;
    inverseKinematics(0, y[0], x[0]);
    x[3] += lDStep/2;
    inverseKinematics(3, y[3], x[3]);

    delay(speed);
  }

  // reset the angle
  angle = 180;
  count = 0;
  rising = true;
  y[1] = height;
  y[2] = height;
  double bx[4] = {x[0] - radius, x[1] - radius, x[2] - radius, x[3] - radius};
  double by[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cx, bx, sizeof(cx));
  memcpy(cy, by, sizeof(cy));

  while(angle > 0){ // loops 10 times
    angle -= 180/archSteps;

    x[0] = cx[0] - radius * cos((angle * 71) / 4068);
    y[0] = cy[0] - (radius * 1.4) * sin((angle * 71) / 4068);
    inverseKinematics(0, y[0], x[0]);

    x[3] = cx[3] - radius * cos((angle * 71) / 4068);
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

    x[1] += lDStep;
    inverseKinematics(1, y[1], x[1]);
    x[2] += lDStep;
    inverseKinematics(2, y[2], x[2]);

    delay(speed);
  }

  // // reset variables
  angle = 180;
  y[0] = height;
  y[3] = height;
  double ax[4] = {x[0] - radius/2, x[1] - radius/2, x[2] - radius/2, x[3] - radius/2};
  double ay[4] = {y[0], y[1], y[2], y[3]};
  memcpy(cx, ax, sizeof(cx));
  memcpy(cy, ay, sizeof(cy));

  while(angle > 180){
    angle -= 180/archSteps;

    x[1] = cx[1] - radius * cos((angle * 71) / 4068);
    y[1] = cy[1] - radius * sin((angle * 71) / 4068);
    inverseKinematics(1, y[1], x[1]);

    x[2] = cx[2] - radius * cos((angle * 71) / 4068);
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

    delay(150);
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

void inverseKinematicsZTest(int legIndex, double torsoHeight, double z){

  double theta = atan( tan( z / torsoHeight ) ) * 57296 / 1000;
  double cNew = sqrt(torsoHeight*torsoHeight + z*z);

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
  
  int hipServoIndex = leg[legIndex][2];
  //the value bellow is meant to be a temporary patch
  //TODO: to set default hip servo pos to 90 degrees
  int hipMiddleAngle = servoRoms[legIndex][4];
  hipMiddleAngle += theta;

  //knee
  HCPCA9685.Servo(kneeServoIndex, map(kneeMin + kneeAngle, 0, 180, 1, 450));

  //femur
  HCPCA9685.Servo(femurServoIndex, map(femurMax - femurAngle, 0, 180, 1, 450));

  //hip
  HCPCA9685.Servo(hipServoIndex, map(hipMiddleAngle, 0, 180, 1, 450));

}

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