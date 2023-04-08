#include "BluetoothSerial.h"

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
      }
    else if (receivedValueBT == '6')
      {
        Serial.write("Six");
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
