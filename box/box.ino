#include <CapacitiveSensor.h>

CapacitiveSensor cap = CapacitiveSensor(6,7);
//pin 7 is receiving pin
unsigned long csSum;

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);
}

void loop() {
  long val = cap.capacitiveSensor(30); //a: Sensor resolution is set to 80
  Serial.println(val);
  delay(10);
  if (val >= 4000) {
    Serial.println("Triggered");
  }
}
