#include <CapacitiveSensor.h>
#include <AccelStepper.h>

// pins for flex sensors on fingers
int fingers[5] = {A0, A1, A2, A3, A4};
bool fingers_check[5] = {false, false, false, false, false};

//half step
#define FULLSTEP 4

// Motor pins
#define motor1  8 // IN1 on the driver
#define motor2  9 // IN2 on the driver
#define motor3  10 // IN3 on the driver
#define motor4  11 // IN4 on the driver

const int STRAIGHT = 800; // volt when straight
const int BEND = 600; // volt at 90 deg

AccelStepper stepper1(FULLSTEP, motor1, motor3, motor2, motor4);

CapacitiveSensor cap = CapacitiveSensor(6,7); //pin 7 is receiving pin

void setup() {
  Serial.begin(9600);

  // declare finger flex sensor pins
  for (int i = 0; i++; i < 5) {
    pinMode(fingers[i], INPUT);
  }
//  pinMode(6, OUTPUT);
//  stepper1.setMaxSpeed(1000.0);
//  stepper1.setAcceleration(100.0);
//  stepper1.setSpeed(300);
}

void loop() {
  for (int i = 0;  i < 5; i++) {
    if (analogRead(fingers[i]) >= 700) {
      fingers_check[i] = true;
    } 
    else {
      fingers_check[i] = false;
    }
  }

  // start game 
  if !(fingers[0] || fingers[1] || fingers[2] || fingers[3] || fingers[4]) {
    
  }
  delay(700);

// motor code
//  long val = cap.capacitiveSensor(30); //a: Sensor resolution is set to 80
//  Serial.println(val);
//  delay(10);
//  if (val >= 4000) {
//    Serial.println("Triggered");
//    long curr = stepper1.currentPosition();
//    long goal = curr + 4096/2;
//    stepper1.runToNewPosition(goal);
//    Serial.println("one rotation");
//  }
}
