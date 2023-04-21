#include "NewPing.h"

#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define TRIGGER_PIN2 5
#define ECHO_PIN2 6

#define MAX_DISTANCE 400  

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("Distance = ");
  Serial.print(sonar.ping_cm());
  Serial.println(" cm");
  Serial.print("Distance2 = ");
  Serial.print(sonar2.ping_cm());
  Serial.println(" cm");
}
