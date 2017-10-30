/*

\/\/\/\/\/\/\/\/\/\/\/\/\
/\/\/\/\/ MASTER \/\/\/\/
\/\/\/\/\/\/\/\/\/\/\/\/\
SWAR 74 - MONASH SUNWAY WARMAN 2017
Timing-based functions 21/4/2017

= Pin Configurations =
-D0(RX) :NONE
-D1(TX) :NONE
-D2   :READY SIGNAL
-D3   :BIN TILTED SIGNAL
-D4   :LEFT MOTOR DIRECTION
-D5   :LEFT MOTOR SPEED
-D6   :RIGHT MOTOR SPEED
-D7   :RIGHT MOTOR DIRECTION
-D8   :LEFT ENCODER YELLOW (UN-USED IN THIS CODE)
-D9   :RIGHT ENCODER YELLOW (UN-USED IN THIS CODE)
-D10  :START TRIGGER
-D11  :SLAVE SIGNAL
-D12  :SONAR ECHO
-D13  :SONAR TRIGGER

-A1   :NONE
-A2   :NONE
-A3   :NONE
-A4   : MAGSENSOR SDA
-A5   : MAGSENSOR SCL

= Functions =
moveFWDS(TIME, LEFT_MOTOR_SPEED, RIGHT_MOTOR_SPEED)
moveREVS(TIME, LEFT_MOTOR_SPEED, RIGHT_MOTOR_SPEED)
turnRightS(TIME)
turnLeftS(TIME)
findBin()
rampBin()
stopHere()

= Sequence =
0. Balls in
1. Wait for 1.5 seconds
2. Go straight for 3 seconds
3. Ask slave to loosen string and wait it rise for 2.5 seconds
4. Turn right for 400ms to face towards the bin
5. Go straight for 5.8 seconds
6. Check if it hit the bin, skip to 9 if yes, continue if no
7. Locate the bin
8. Ramp the bin, stop when the bin tilted and hit the limit switch //max time
9. Reverse for a 250ms
10.Turn left for 1.5 seconds to face towards the bin
11.Go straight for 5 seconds
12.Check if it hit the bin, skip to 14 if yes, continue if no
13.Locate the bin
14.Ramp the bin
15.Finish - yay

*/


/* Include library */
#include <NewPing.h>




/* Pin Configuration */
//1 is left
#define readyPin 2
#define binPin 3
#define left_motor 4
#define left_motor_speed 5
#define right_motor_speed 6
#define right_motor 7
#define enc_Left_Yellow 8
#define enc_Right_Yellow 9
#define startPin 10
#define relayPin 11
#define ECHO_PIN 12
#define TRIGGER_PIN 13




/* Global Variables */
int ballIn = LOW;

//Constants configuration
#define FWD HIGH //HIGH is foward
#define REV LOW //HIGH is reverse
#define stopspeed 0
#define delayTiming 100 //in milli seconds
#define MAX_DISTANCE 50 //in cm
#define MAX_RAMP_TIME 5000




/* Setup I/O ports for Arduino board */
void setup()
{
  Serial.begin(9600);
  pinMode(left_motor, OUTPUT);
  pinMode(right_motor, OUTPUT);
  pinMode(left_motor_speed, OUTPUT);
  pinMode(right_motor_speed, OUTPUT);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(readyPin, OUTPUT);
  pinMode(binPin, INPUT_PULLUP);
}


/* MAIN PROGRAM LOOP */
void loop()
{

  Serial.println("Arduino ONLINE.");
  delay(3000); //delayed start
  digitalWrite(relayPin, LOW);
  digitalWrite(readyPin, HIGH);
  ballIn = digitalRead(startPin);
  while (ballIn == LOW) {
    delay(20);
    ballIn = digitalRead(startPin);
  }

  digitalWrite(readyPin, LOW);
  Serial.println("Ores in.");

  delay(1500);

  Serial.println("Start moving.");
  //max speed: 255
  moveFWDS(3000, 255, 255); //moveFWDS(TIME, LEFT_SPEED, RIGHT_SPEED
  delay(100);

  //signal 2nd arduino
  Serial.println("Ask 2nd ARDUINO to loosen the rope.");
  digitalWrite(relayPin, HIGH); //LOW iS DISABLE
  delay(2500);

  Serial.println("turn toward 1st bin");
  turnRightS(400);
  delay(300);
  Serial.println("move towards 1st bin");
  moveFWDS(5700, 255, 255);
  digitalWrite(relayPin, LOW);
  /*
  if (digitalRead(binPin)) {
  //seems like the rover hit the bin accidentally
  goto next; //skip to next operation
  }
  */
  Serial.println("find 1st bin");
  findBin();
  Serial.println("ramp 1st bin");
  rampBin();
  //next:
  Serial.println("reverse");
  moveREVS(250, 180, 180);

  Serial.println("turn towards 2nd bin");
  turnLeftS(1750);
  Serial.println("move towards 2nd bin");
  moveFWDS(4100, 255, 255);
  /*
  if (digitalRead(binPin)) {
  //seems like the rover hit the bin accidentally
  goto finishLine; //skip to next operation
  }
  */
  Serial.println("find 2nd bin");
  findBin();
  Serial.println("ramp 2nd bin");
  rampBin();
  //finishLine:

  //Prevent the whole loop from running again
  stopHere();
}

void moveFWDS(int x, int leftSpeed, int rightSpeed) {
  digitalWrite(right_motor, FWD); //Forward 
  digitalWrite(left_motor, FWD); //Forward
  analogWrite(right_motor_speed, rightSpeed);//180 //175
  analogWrite(left_motor_speed, leftSpeed);//180 //230
  delay(x);
  analogWrite(right_motor_speed, stopspeed); // stop motors
  analogWrite(left_motor_speed, stopspeed);
  delay(delayTiming);
}

void moveREVS(int x, int leftSpeed, int rightSpeed) {
  digitalWrite(right_motor, REV); //Backward 
  digitalWrite(left_motor, REV); //Backward
  analogWrite(right_motor_speed, rightSpeed); //100
  analogWrite(left_motor_speed, leftSpeed); //220
  delay(x);
  analogWrite(right_motor_speed, stopspeed);      // stop motors
  analogWrite(left_motor_speed, stopspeed);      // stop motors
  delay(delayTiming);
}

void turnLeftS(int x) {
  digitalWrite(left_motor, REV);
  digitalWrite(right_motor, FWD);
  analogWrite(left_motor_speed, 245);
  analogWrite(right_motor_speed, 245);
  delay(x);
  analogWrite(right_motor_speed, stopspeed);      // stop motors
  analogWrite(left_motor_speed, stopspeed);      // stop motors
  delay(delayTiming);
}

void turnRightS(int x) {
  digitalWrite(left_motor, FWD);
  digitalWrite(right_motor, REV);
  analogWrite(left_motor_speed, 180);
  analogWrite(right_motor_speed, 180);
  delay(x);
  analogWrite(right_motor_speed, stopspeed);      // stop motors
  analogWrite(left_motor_speed, stopspeed);      // stop motors
  delay(delayTiming);
}

void stopHere() {
  digitalWrite(relayPin, LOW); //LOW iS DISABLE
stophere:
  delay(100000);
  goto stophere;
}

void findBin() {
  delay(500);
  int distance = 0;
  //TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE
  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
  //turn Right
  digitalWrite(left_motor, FWD);
  digitalWrite(right_motor, REV);
  analogWrite(left_motor_speed, 200);
  analogWrite(right_motor_speed, 200);
  delay(1200);

  //check if it is right enough
  distance = sonar.ping() / US_ROUNDTRIP_CM;
  if (distance != 0) {
    delay(500); //turn right for 0.5sec more if it isnt enough
  }

  analogWrite(left_motor_speed, stopspeed);
  analogWrite(right_motor_speed, stopspeed);
  delay(500);
  distance = sonar.ping() / US_ROUNDTRIP_CM;
  boolean binLocated = false;

  //turn left
  digitalWrite(left_motor, REV);
  digitalWrite(right_motor, FWD);
  do {
    analogWrite(left_motor_speed, 245);
    analogWrite(right_motor_speed, 245);

    distance = sonar.ping() / US_ROUNDTRIP_CM;
    if (distance != 0) {
      //stop motor and check again 
      analogWrite(left_motor_speed, stopspeed);
      analogWrite(right_motor_speed, stopspeed);
      delay(300);
      distance = sonar.ping() / US_ROUNDTRIP_CM;
      if (distance != 0) {
        analogWrite(left_motor_speed, 245);
        analogWrite(right_motor_speed, 245);
        delay(350);
        analogWrite(left_motor_speed, stopspeed);
        analogWrite(right_motor_speed, stopspeed);
        if (distance == 0) {
          //overturned, gg
          //turn to right again
          digitalWrite(left_motor, FWD);
          digitalWrite(right_motor, REV);
          analogWrite(left_motor_speed, 245);
          analogWrite(right_motor_speed, 245);
          delay(175);
          analogWrite(left_motor_speed, stopspeed);
          analogWrite(right_motor_speed, stopspeed);
        } //FWD IS 1, REV IS 0

          /*
          bin located but too far away
          move rover closer to the bin
          */
        digitalWrite(left_motor, FWD);
        digitalWrite(right_motor, FWD);
        while (distance > 20) {
          analogWrite(left_motor_speed, 245);
          analogWrite(right_motor_speed, 245);
          distance = sonar.ping() / US_ROUNDTRIP_CM; //recalculate distance
        }

        analogWrite(left_motor_speed, stopspeed);
        analogWrite(right_motor_speed, stopspeed);
        binLocated = true;
      }
    }
  } while (!binLocated);
}

void rampBin() {
  delay(500);
  //MAX_RAMP_TIME
  unsigned long time = millis();
  digitalWrite(left_motor, FWD);
  digitalWrite(right_motor, FWD);
  analogWrite(left_motor_speed, 110);
  analogWrite(right_motor_speed, 110);
  boolean binTilted = false;
  while (1) {
    delay(50);
    binTilted = digitalRead(binPin);
    if (binTilted || (millis() - time > MAX_RAMP_TIME))
      break;
  }
  analogWrite(left_motor_speed, stopspeed);
  analogWrite(right_motor_speed, stopspeed);
  delay(700);
}