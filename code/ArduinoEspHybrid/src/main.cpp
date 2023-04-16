#include <Arduino.h>
//--- for mac addres obtaining
#include <WiFi.h>
//---

//--- for walking
//---

void setup(){
  //--= for mac addres obtaining
  Serial.begin(115200);
  Serial.println();
  Serial.print("Your ESP Board MAC Address is:  ");
  Serial.println(WiFi.macAddress());
  //---
}
void loop(){
}