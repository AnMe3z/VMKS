#include "BluetoothSerial.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

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
  if (Serial.available()) 
  {
    if(Serial.read() == 49)
      {
        for(int i = 65; i <= 90; i++) 
        {
          Serial.write((char)i);
          SerialBT.write((char)i);
          delay(50);
        }
      }
  }
  if (SerialBT.available()) 
  {
    if(SerialBT.read() == 49)
    {
      for(int i = 65; i <= 90; i++) 
      {
        Serial.write((char)i);
        SerialBT.write((char)i);
        delay(50);
      }
    }
  }
  delay(20);
}
