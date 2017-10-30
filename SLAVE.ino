/*

\/\/\/\/\/\/\/\/\/\/\/\/\
/\/\/\/\/ _SLAVE \/\/\/\/
\/\/\/\/\/\/\/\/\/\/\/\/\
SWAR 74 - MONASH SUNWAY WARMAN 2017
Servo code - Winder that controll the delivery arm.

= Pin Configurations =
-D0(RX) :NONE
-D1(TX) :NONE
-D2   :NONE
-D3   :SERVO ATTACH PIN
-D4   :SIGNAL FROM MASTER
-D5   :NONE
-D6   :NONE
-D7   :NONE
-D8   :NONE
-D9   :NONE
-D10  :NONE
-D11  :NONE
-D12  :NONE
-D13  :SIGNAL FROM DELIVERY ARM

-A1   :NONE
-A2   :NONE
-A3   :NONE
-A4   :NONE
-A5   :NONE

= Sequence =
0. WAIT FOR MASTER SIGNAL TO LOOSEN THE STRING
1. LOOSEN THE STRING UNTIL IT HITS THE LIMIT SWITCH
2. STOP SERVO AND GO BACK TO STEP 0

*/

/* Include library */
#include <Servo.h>

#define servoAttachPin 3
#define signalFromMaster 4
#define signalFromStick 13

boolean servoON = 0;
boolean stickSignal = 0;
boolean masterSignal = 0;
boolean signalReceived = 0;
unsigned long time;
int counter=0;

Servo windServo; //Create a servo object

void setup() {
	pinMode(signalFromMaster, INPUT);
	pinMode(signalFromStick, INPUT_PULLUP);
	Serial.begin(9600);
}

void loop() {
	Serial.println("SLAVE ONLINE");
	delay(3000);
	Serial.println("SLAVE READY");

startPoint:
	signalReceived = 0;

	time = millis();

	while (!signalReceived) {
		counter = counter + 1;
		if (counter>50) {
			Serial.println("Heartbeat");
			counter=0;
		}
		masterSignal = digitalRead(signalFromMaster);
		/*
			if this loop see a signal, wait 300ms and check if the signal is 
			still there. if yes, exit loop else continue check signal
		*/
		if (masterSignal == 1) {
			delay(300);
			masterSignal = digitalRead(signalFromMaster);
			if (masterSignal == 1) {
				break;
			}
		}
		delay(50);
	}

	Serial.print("Master signal (Outside while): ");
	Serial.println(masterSignal);
	while (masterSignal == 1) { //if Master wants to wind the rope
		Serial.print("Inside master signal loop, stickSignal: ");
		stickSignal = digitalRead(signalFromStick);
		Serial.println(stickSignal);
		while (stickSignal == 0) { //if the stick hasn't touched the limit switch yet
			Serial.println("Inside stick signal loop, ");
			if (servoON == 0) {
				Serial.println("Servo is not on, turn it on:");
				windServo.attach(servoAttachPin); //turn on servo if it isn't already on
				servoON = 1;
			}
			Serial.println("Servo is already on:");
			stickSignal = digitalRead(signalFromStick);
		}
		windServo.detach(); //detach servo when the stick touched the limit switch
		servoON = 0;
		Serial.println("stick signal = 1 turned servo off");
		masterSignal = digitalRead(signalFromMaster);
	}
	if (servoON == 1 && (masterSignal == 0 || stickSignal == 0)) {
		windServo.detach(); //detach servo when the stick touched the limit switch
		servoON = 0;
	}
	goto startPoint;
}