#include "BluetoothSerial.h"


#include <Wire.h>

#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

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
void inverseKinematics(int legIndex, double torsoHeight, double x);
double cosineTheorem(int a, int b, double c);

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";
char receivedValue; //debugging
char receivedValueBT;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() 
{
  //driver initialization
  board1.begin(); 
  board1.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
}

void loop() 
{
  // if (Serial.available()) 
  // {
  //   receivedValue = Serial.read();
  //   if (receivedValue == '1')
  //     {
  //       Serial.write("One");
  //     }
  //   else if (receivedValue == '2')
  //     {
  //       Serial.write("Two");
  //     }
  //   else if (receivedValue == '3')
  //     {
  //       Serial.write("Three");
  //     }
  //   else if (receivedValue == '4')
  //     {
  //       Serial.write("Four");
  //     }
  // }
  //debug purposes
  
  standUp(5);
  if (SerialBT.available()) 
  {
    receivedValueBT = SerialBT.read();
    if (receivedValueBT == '1')
      {
        Serial.write("One");
      }
    else if (receivedValueBT == '2')
      {
        Serial.write("Two");
      }
    else if (receivedValueBT == '3')
      {
        Serial.write("Three");
      }
    else if (receivedValueBT == '4')
      {
        Serial.write("Four");
      }
    else if (receivedValueBT == '5')
      {
        Serial.write("Five");
        standUp(5);
      }
    else if (receivedValueBT == '6')
      {
        Serial.write("Six");
        standDown();
      }
    else if (receivedValueBT == '7')
      {
        Serial.write("Seven");
      }
    else if (receivedValueBT == '8')
      {
        Serial.write("Eight");
      }
    else if (receivedValueBT == '9')
      {
        Serial.write("Nine");
      }

  }
  delay(20);
}

// --- complex movements

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
  // y[2] -= 1;
  // y[3] -= 1;
  // inverseKinematics(2, y[2], x[2]);
  // inverseKinematics(3, y[3], x[3]);
  // delay(speed);
  
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

    delay(150);
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
  board1.setPWM(kneeServoIndex, 0, angleToPulse(kneeMin + kneeAngle));
  
  //femur
  board1.setPWM(femurServoIndex, 0, angleToPulse(femurMax - femurAngle));

  //hip
  int hipServoIndex = leg[legIndex][2];
  //the value bellow is meant to be a temporary patch
  //TODO: to set default hip servo pos to 90 degrees
  int hipMiddleAngle = servoRoms[legIndex][4];
  board1.setPWM(hipServoIndex, 0, angleToPulse(hipMiddleAngle));

}

int angleToPulse(int ang){
   int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
   return pulse;
}

double cosineTheorem(int a, int b, double c){
  if (c == 0){
    return 0;
  }
  else{
    return acos((b * b + a * a - c * c)/(2.0 * b * a)) * 57296 / 1000;
  }
}

