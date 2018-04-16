#include <CapacitiveSensor.h>

//LED's
int led[3] = {A5, 8, 9};
//int ledCap = 1;
//int ledFing = 0;

// pins for flex sensors on fingers
int fingers[5] = {A0, A1, A2, A3, A4};

int fingerThresh[5] = {770, 750, 765, 740, 760};
bool fingersCheck[5] = {false, false, false, false, false};
int gameFingers[3][5] = {
 // {false, false, false, false, false}, //open hand
  {true, true, true, true, true}, //fist
  {true, false, false, true, true}, //peace
  {false, false, true, true, false}, //devil horns
 // {false, true, true, true, false}, //hang loose
};

// Motor pins
#define motor1  10 // IN1 on the driver
#define motor2  11 // IN2 on the driver
#define motor3  12 // IN3 on the driver
#define motor4  13 // IN4 on the driver
int motorDelay = 2;

const int STRAIGHT = 800; // volt when straight
const int BEND = 600; // volt at 90 deg

int ledDelay[2] = {700, 250};
int delayIndex = 0;

CapacitiveSensor game1Cap = CapacitiveSensor(2,3); //pin 3 receive - game 1
CapacitiveSensor game2Cap = CapacitiveSensor(4,5); //pin 5 receive - game 2
CapacitiveSensor game3Cap = CapacitiveSensor(6,7); //pin 7 receive - game 3

CapacitiveSensor caps[3] = {game1Cap, game2Cap, game3Cap};

unsigned long capSum;

int gameNum = 0;
bool debug = false;
bool finger = false;
bool testMotor = false;
bool testCap = false;
int capTest = 0;
int fingerPrint = 4;

void setup() {
  Serial.begin(9600);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  pinMode(motor4, OUTPUT);
  pinMode(led[0], OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[2], OUTPUT);

  // declare finger flex sensor pins
  for (int i = 0; i++; i < 5) {
    pinMode(fingers[i], INPUT);
  }
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
  else if (testCap) {
    checkCap(capTest);
  }
  else if (gameNum < 3) {
    digitalWrite(A5, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    for (int i = 0; i < 3; i++) {
      delayIndex = 0;
      Serial.println("start game ");
      Serial.println(i);
      checkGame(i);
      turnMotors(3 + gameNum);
    }
  }
}

void checkGame(int gameNum) {
  bool cap = false;
  bool finger = false;
  while (!cap) {
    cap = checkCap(gameNum);
  }
  Serial.println("cap");
  delayIndex += 1;
  while (!finger) {
    flashLED(led[gameNum], delayIndex);
    finger = checkFingers(gameNum);
  }
  Serial.println("finger");
  Serial.println("done checking");
  digitalWrite(led[gameNum], HIGH);
  Serial.println(gameNum);
  gameNum += 1;
}

bool checkFingers(int game_num) {
  Serial.println(game_num);
  bool corr = true;
  for (int i = 0; i < 5; i++) {
    if ((gameFingers[game_num][i]) != (analogRead(fingers[i]) <= (fingerThresh[i]))) {
      corr = false;
      Serial.println("missing: " + i);
    }
  }
  if (corr) {
    delay(500);
    for (int i = 0; i < 5; i++) {
      if ((gameFingers[game_num][i]) != (analogRead(fingers[i]) <= (fingerThresh[i]))) {
        corr = false;
        Serial.println("missing: " + i);
      }
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
  Serial.println(analogRead(fingers[finger]) <= (fingerThresh[finger]));
  delay(100);
}

bool checkCap(int capNum) {
  bool triggered = false;
  while (!triggered) {
    long capRead = caps[capNum].capacitiveSensor(30);
    if (capRead > 150) {
      capSum += capRead;
      if (capSum >= 500)
      {
        Serial.print("Trigger: ");
        Serial.println(capSum);
        if (capSum > 0) {
          capSum = 0;
        }
        caps[capNum].reset_CS_AutoCal(); //Stops readings
        triggered = true;
      }
    } else {
      capSum = 0; //Timeout caused by bad readings
    }
  }
  return true;
}

void turnMotors(int turns) {
  for (int i = 0; i <= turns * 1000; i++) {
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
    digitalWrite(motor3, LOW);
    digitalWrite(motor4, LOW);
    delay(motorDelay);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, HIGH);
    digitalWrite(motor3, LOW);
    digitalWrite(motor4, LOW);
    delay(motorDelay);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    digitalWrite(motor3, HIGH);
    digitalWrite(motor4, LOW);
    delay(motorDelay);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    digitalWrite(motor3, LOW);
    digitalWrite(motor4, HIGH);
    delay(motorDelay);
  }
}

void flashLED(int led, int delayIndex) {
  digitalWrite(led, HIGH);
  delay(ledDelay[delayIndex]);
  digitalWrite(led, LOW);
  delay(ledDelay[delayIndex]);
}

