#include <CapacitiveSensor.h>
#include <AccelStepper.h>

// pins for flex sensors on fingers
int fingers[5] = {A0, A1, A2, A3, A4};

int fingerThresh[5] = {770, 750, 765, 740, 750};
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
#define motor1  10 // IN1 on the driver
#define motor2  11 // IN2 on the driver
#define motor3  12 // IN3 on the driver
#define motor4  13 // IN4 on the driver

const int STRAIGHT = 800; // volt when straight
const int BEND = 600; // volt at 90 deg

AccelStepper stepper1(FULLSTEP, motor1, motor3, motor2, motor4);

CapacitiveSensor startCap = CapacitiveSensor(0,1); //pin 1 receive - start game palm 
CapacitiveSensor game1Cap = CapacitiveSensor(2,3); //pin 3 receive - game 1
CapacitiveSensor game2Cap = CapacitiveSensor(4,5); //pin 5 receive - game 2
CapacitiveSensor game3Cap = CapacitiveSensor(6,7); //pin 7 receive - game 3

CapacitiveSensor caps[3] = {game1Cap, game2Cap, game3Cap};

unsigned long capSum;

int gameNum = 0;
bool debug = false;
bool finger = false;
bool testMotor = true;
int fingerPrint = 4;

void setup() {
  Serial.begin(9600);

  // declare finger flex sensor pins
  for (int i = 0; i++; i < 5) {
    pinMode(fingers[i], INPUT);
  }
//  code for motor

  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(300);
}

void loop() {
  if (debug) {
    debugFingers(1);
  }
  else if (finger) {
    printFinger(fingerPrint);
  }
  else if (testMotor) {
    turnMotors(1);
  }
  else {
    for (int i = 0; i < 3; i++) {
      Serial.println("start game ");
      Serial.println(i);
      checkGame(i);
      turnMotors(1);
    }
  
    delay(400);
  }
}

void checkGame(int gameNum) {
  bool cap = false;
  bool finger = false;
  while (!(cap && finger)) {
    cap = checkCap(gameNum);
    finger = checkFingers(gameNum);
    if (finger) {
      Serial.println("finger");
    }
  }
  Serial.println("done checking");
  Serial.println(gameNum);
  gameNum += 1;
}

bool checkFingers(int game_num) {
  Serial.println("fingers game");
  Serial.println(game_num);
  bool corr = true;
  for (int i = 0;  i < 5; i++) {
    if ((gameFingers[game_num][i]) != (analogRead(fingers[i]) <= (fingerThresh[i]))) {
      corr = false;
    } 
  }
  return corr;
}

bool debugFingers(int capNum) {
  for (int i = 0;  i < 5; i++) {
    if (analogRead(fingers[i]) <= (fingerThresh[i])) {
      fingersCheck[i] = true;
    } 
    else {
      fingersCheck[i] = false;
    }
  }
  bool check = checkCap(capNum);
  if (check) {
    for (int i = 0;  i < 5; i++) {
      if (fingersCheck[i]) {
        Serial.println(i);
      } 
    }
  }
}

void printFinger(int finger) {
  Serial.println(analogRead(fingers[finger]));
  delay(100);
}

bool checkCap(int capNum) {
  long capRead = caps[capNum].capacitiveSensor(80);
  if (capRead > 100) {
    capSum += capRead;
    if (capSum >= 500)
    {
      Serial.print("Trigger: ");
      Serial.println(capSum);
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

void turnMotors(int turns) {
  for (int i = 0; i <= turns * 512; i++) {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(delayTime);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(delayTime);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(delayTime);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(delayTime);
  }
}

