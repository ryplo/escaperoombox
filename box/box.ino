#include <CapacitiveSensor.h>
#include <AccelStepper.h>

// pins for flex sensors on fingers
int fingers[5] = {A0, A1, A2, A3, A4};
bool fingersCheck[5] = {false, false, false, false, false};
int gameFingers[5][5] = {
  {false, false, false, false, false}, //open hand
  {true, true, true, true, true}, //fist
  {true, false, false, true, true}, //peace
  {false, false, true, true, false}, //devil horns
  {false, true, true, true, false}, //hang loose
};

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

CapacitiveSensor startCap = CapacitiveSensor(0,1); //pin 1 receive - start game palm 
CapacitiveSensor game1Cap = CapacitiveSensor(2,4); //pin 3 receive - game 1
CapacitiveSensor game2Cap = CapacitiveSensor(4,5); //pin 5 receive - game 2
CapacitiveSensor game3Cap = CapacitiveSensor(6,7); //pin 7 receive - game 3

CapacitiveSensor caps[4] = {startCap, game1Cap, game2Cap, game3Cap};

unsigned long capSum;

int gameNum = 0;

void setup() {
  Serial.begin(9600);

  // declare finger flex sensor pins
  for (int i = 0; i++; i < 5) {
    pinMode(fingers[i], INPUT);
  }
//  code for motor
//  pinMode(6, OUTPUT);
//  stepper1.setMaxSpeed(1000.0);
//  stepper1.setAcceleration(100.0);
//  stepper1.setSpeed(300);
}

void loop() {
  checkGame(gameNum);
  Serial.println("done");
  
  delay(400000);
// motor code
//    long curr = stepper1.currentPosition();
//    long goal = curr + 4096/2;
//    stepper1.runToNewPosition(goal);
//    Serial.println("one rotation");
}

void checkGame(int gameNum) {
  bool cap = false;
  bool finger = false;
  while (!(cap && finger)) {
    cap = checkCap(1);
    finger = checkFingers(gameNum);
    if (finger) {
      Serial.println("finger");
    }
  }
  Serial.println("done checking");
}

bool checkFingers(int game_num) {
  bool corr = true;
  for (int i = 0;  i < 5; i++) {
    if ((gameFingers[game_num][i]) != (analogRead(fingers[i]) <= 700)) {
      corr = false;
    } 
  }
  return corr;
}

bool checkCap(int capNum) {
  long capRead = caps[capNum].capacitiveSensor(80);
  if (capRead > 100) {
    capSum += capRead;
    if (capSum >= 3800)
    {
      Serial.print("Trigger: ");
      Serial.println(capSum);
      Serial.println("fingers:");
      if (capSum > 0) {
        capSum = 0;
      }
      caps[capNum].reset_CS_AutoCal(); //Stops readings
      return true;
    }
  } else {
    capSum = 0; //Timeout caused by bad readings
    return false;
  }
}

