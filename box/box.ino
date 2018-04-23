#include <CapacitiveSensor.h>

//LED's
int led[3] = {A5, 8, 9};

// pins for flex sensors on fingers
int fingers[5] = {A0, A1, A2, A3, A4};

int fingerThresh[5] = {770, 750, 765, 740, 760}; // volt at 90 deg
bool fingersCheck[5] = {false, false, false, false, false};
int gameFingers[3][5] = {
  {true, true, true, true, true}, //fist
  {true, false, false, true, true}, //peace
  {false, false, true, true, false}, //love
};

// Motor pins
#define motor1  10 // IN1 on the driver
#define motor2  11 // IN2 on the driver
#define motor3  12 // IN3 on the driver
#define motor4  13 // IN4 on the driver
int motorDelay = 2;

// Delay between LED on/off for flashing effect. 
int ledDelay = 250;

// Defining capacitive sensors using CapacitiveSensor library
CapacitiveSensor game1Cap = CapacitiveSensor(2,3); //pin 3 receive - game 1
CapacitiveSensor game2Cap = CapacitiveSensor(4,5); //pin 5 receive - game 2
CapacitiveSensor game3Cap = CapacitiveSensor(6,7); //pin 7 receive - game 3

CapacitiveSensor caps[3] = {game1Cap, game2Cap, game3Cap};

// Adding up readings from capacitive sensor
unsigned long capSum;

// Keeping track of which round/puzzle the user is on
int globalGameNum = 0;

// Flags for debugging - set to true to run debug/testing code
bool debug = false;
bool finger = false;
bool testMotor = false;
bool testCap = false;
// Change these values to test different capacitive sensors or flex sensors on debug mode.
int capTest = 0;
int fingerPrint = 4;


// ****************************************************************************************
// ****************************************************************************************
// TEST/MOVE MOTOR FLAGS: use these to choose what the motors do 
// only set one of these to 1 at a time.
// change code, plug arduino in, upload and unplug
bool cont_up = 0; // move continuously up - does not stop until you unplug!!
bool cont_down = 0; // move continuously down - does not stop until you unplug!!
bool set_up = 0; // move the set amount up - stops for about 1000 seconds when done.
bool set_down = 0; // move the set amount down - stops for about 1000 seconds when done.

// The number of turns, ordered.
// These values are shared by the actual game code as well as the test/move motors code. 
int turns[3] = {2, 2, 12};
// ****************************************************************************************
// ****************************************************************************************

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
  // Debug flex sensors
  if (debug) {
    debugFingers(1);
  }
  // Debug flex sensors - print readings
  else if (finger) {
    printFinger(fingerPrint);
  }
  // Test motor - runs 10 cycles
  else if (testMotor) {
    turnMotorsUp(10);
  }
  // Test capacitive sensor
  else if (testCap) {
    checkCap(capTest);
  }
  else if (cont_up) {
    up();
  }
  else if (cont_down) {
    down();
  }
  else if (set_up) {
    // turn up to ending level - pauses briefly at each stop, and pauses for a long time when done
    // make sure to unplug after it's moved down all three stops otherwise it will loop
    // and continue moving
    for (int i = 0; i < 3; i++) {
      turnMotorsUp(turns[i]);
      delay(1000);
    }
    delay(1000000);
  }
  else if (set_down) {
    // turn down to starting level - pauses briefly at each stop, and pauses for long time when done,
    // make sure to unplug after it's moved down all three stops otherwise it will loop
    // and continue moving
    for (int i = 2; i >= 0; i--) {
      turnMotorsDown(turns[i]);
      delay(1000);
    }
    delay(1000000);
  }
  // Play game
  else if (globalGameNum < 3) {
    digitalWrite(A5, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    for (int i = 0; i < 3; i++) {
      Serial.println("start game ");
      Serial.println(i);
      checkGame(i);
      Serial.println("gameNum");
      Serial.println(globalGameNum);
      turnMotorsUp(turns[i]);
      digitalWrite(led[i], HIGH);
    }
  }
}

// Checks capacitive sensors and flex sensors for correct input
void checkGame(int gameNum) {
  bool cap = false;
  bool finger = false;
  while (!cap) {
    cap = checkCap(gameNum);
  }
  Serial.println("cap");
  while (!finger) {
    flashLED(led[gameNum]);
    finger = checkFingers(gameNum);
  }
  Serial.println("finger");
  Serial.println("done checking");
  Serial.println(gameNum);
  globalGameNum += 1;
}

// Reads input from flex sensors and compares to values required to pass the round
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

// Reads input from capacitive sensors and compares to values required to pass the round
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

// CCW - up
void turnMotorsUp(int no_turns) {
  Serial.println("turns: ");
  Serial.println(no_turns);
  for (int i = 0; i <= no_turns * 1000; i++) {
    up();
  }
}

void turnMotorsDown(int no_turns) {
  for (int i = 0; i <= no_turns * 1000; i++) {
    down();
  }
}

// Moves motor with 4-step sequence
void up() {
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

void down() {
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);
  delay(motorDelay);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
  delay(motorDelay);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);
  delay(motorDelay);
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);
  delay(motorDelay);
}

void flashLED(int led) {
  digitalWrite(led, HIGH);
  delay(ledDelay);
  digitalWrite(led, LOW);
  delay(ledDelay);
}

// Debugging method - prints out which fingers are flexed upon tapping a capacitive sensor
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

// Debugging method - prints out all bent flex sensors
void printFinger(int finger) {
  Serial.println(analogRead(fingers[finger]) <= (fingerThresh[finger]));
  delay(100);
}
