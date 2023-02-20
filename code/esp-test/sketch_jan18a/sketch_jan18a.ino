byte receivedData;

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup()
{
    // Serial monitor setup
    Serial.begin(115200);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
}
 
void loop()
{

   if(Serial.available() > 0){
      receivedData = Serial.read();
      if(receivedData == (char)49){
        for(int i = 65; i <= 90; i++) {
          Serial.print((char)i);
          delay(50);
        }
      }
   }

   delay(200);

}
