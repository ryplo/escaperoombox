#include <CapacitiveSensor.h>
#include <AccelStepper.h>

//half step
#define FULLSTEP 4
// Motor pins
#define motor1  8 // IN1 on the driver
#define motor2  9 // IN2 on the driver
#define motor3  10 // IN3 on the driver
#define motor4  11 // IN4 on the driver

AccelStepper stepper1(FULLSTEP, motor1, motor3, motor2, motor4);

CapacitiveSensor cap = CapacitiveSensor(6,7); //pin 7 is receiving pin

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(300);
}

void loop() {
  long val = cap.capacitiveSensor(30); //a: Sensor resolution is set to 80
  Serial.println(val);
  delay(10);
  if (val >= 4000) {
    Serial.println("Triggered");
    long curr = stepper1.currentPosition();
    long goal = curr + 4096/2;
    stepper1.runToNewPosition(goal);
    Serial.println("one rotation");
  }
}
