#include <Renard.h>
//https://learn.adafruit.com/neopixels-and-servos/the-ticoservo-library
//#include "Adafruit_TiCoServo.h"
#include <Servo.h>

#define DRUM_1_PIN 2
#define DRUM_2_PIN 3
#define DRUM_3_PIN 4
#define DRUM_4_PIN 5
#define DRUM_5_PIN 6

#define SKELETON_PIN_HEAD_1_EYES 7
#define SKELETON_PIN_HEAD_1_SERVO 8

#define SKELETON_PIN_HEAD_2_EYES 11
#define SKELETON_PIN_HEAD_2_SERVO 12

#define GUITAR_PIN_RED 22
#define GUITAR_PIN_BLUE 24
#define GUITAR_PIN_GREEN 26
#define GUITAR_PIN_WHITE 28

#define SMOKE_LEFT_PIN 30
#define SMOKE_RIGHT_PIN 32

#define GUITAR_PIN_HAND_STEP 50
#define GUITAR_PIN_HAND_DIRECTION 52
#define GUITAR_PIN_LIMIT_SENSOR A7

#define GUITAR_HAND_MIN 0
#define GUITAR_HAND_MAX 800
#define LIMIT_SENSOR_TRIPPED 100
int currentHandPosition = GUITAR_HAND_MIN;
int destinationPosition = currentHandPosition;
bool areWeCurrentlyMovingTheHand = false;
bool handIsZeroed = false;

//Down is toward the body of the guitar. Your movingyour hand "up" the neck of the guitar
#define HAND_UP HIGH
#define HAND_DOWN LOW
#define PULSE_BETWEEN_STEPPER_STEPS 20
#define MILLS_BETWEEN_STEPS 2
long lastTimeWeMovedMotor = 0;

Servo skeleton1Servo;
Servo skeleton2Servo;

Renard renard(Serial, 15);

int prevSkeletonServo1Pos = 90;
int prevSkeletonServo2Pos = 90;

void setup()
{

	Serial.begin(57600);

	pinMode(DRUM_1_PIN, OUTPUT);
	pinMode(DRUM_2_PIN, OUTPUT);
	pinMode(DRUM_3_PIN, OUTPUT);
	pinMode(DRUM_4_PIN, OUTPUT);
	pinMode(DRUM_5_PIN, OUTPUT);

	digitalWrite(DRUM_1_PIN, LOW);
	digitalWrite(DRUM_2_PIN, LOW);
	digitalWrite(DRUM_3_PIN, LOW);
	digitalWrite(DRUM_4_PIN, LOW);
	digitalWrite(DRUM_5_PIN, LOW);

	pinMode(GUITAR_PIN_RED, OUTPUT);
	pinMode(GUITAR_PIN_BLUE, OUTPUT);
	pinMode(GUITAR_PIN_GREEN, OUTPUT);
	pinMode(GUITAR_PIN_WHITE, OUTPUT);

	pinMode(GUITAR_PIN_HAND_DIRECTION, OUTPUT);
	pinMode(GUITAR_PIN_HAND_STEP, OUTPUT);
	digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_UP); //moving UP

	pinMode(SKELETON_PIN_HEAD_1_EYES, OUTPUT);
	skeleton1Servo.attach(SKELETON_PIN_HEAD_1_SERVO);
	skeleton1Servo.write(90);

	pinMode(SKELETON_PIN_HEAD_2_EYES, OUTPUT);
	skeleton2Servo.attach(SKELETON_PIN_HEAD_2_SERVO);
	skeleton2Servo.write(90);

	pinMode(SMOKE_LEFT_PIN, OUTPUT);
	pinMode(SMOKE_RIGHT_PIN, OUTPUT);

	lastTimeWeMovedMotor = millis();
}
void loop()
{
	moveHand();
	if (renard.processInput())
	{

		int brightness = 0;

		//Fix for drums with the new mini arduino
		//This makes any value over 80 be high, else low
		//somy sequences while programming look nowmal inthe preview
		brightness = renard.channelValue(1);
		digitalWrite(DRUM_1_PIN, brightness > 200 ? HIGH : LOW);

		brightness = renard.channelValue(2);
		digitalWrite(DRUM_2_PIN, brightness > 200 ? HIGH : LOW);

		brightness = renard.channelValue(3);
		digitalWrite(DRUM_3_PIN, brightness > 200 ? HIGH : LOW);

		brightness = renard.channelValue(4);
		digitalWrite(DRUM_4_PIN, brightness > 200 ? HIGH : LOW);

		brightness = renard.channelValue(5);
		digitalWrite(DRUM_5_PIN, brightness > 200 ? HIGH : LOW);

		if (renard.channelValue(6) >= 127)
		{
			digitalWrite(GUITAR_PIN_RED, HIGH);
			randomlyPickHandLocation();
		}
		else
		{
			digitalWrite(GUITAR_PIN_RED, LOW);
		}

		if (renard.channelValue(7) >= 127)
		{
			digitalWrite(GUITAR_PIN_BLUE, HIGH);
			randomlyPickHandLocation();
		}
		else
		{
			digitalWrite(GUITAR_PIN_BLUE, LOW);
		}

		if (renard.channelValue(8) >= 127)
		{
			digitalWrite(GUITAR_PIN_GREEN, HIGH);
			randomlyPickHandLocation();
		}
		else
		{
			digitalWrite(GUITAR_PIN_GREEN, LOW);
		}

		if (renard.channelValue(9) >= 127)
		{
			digitalWrite(GUITAR_PIN_WHITE, HIGH);
			randomlyPickHandLocation();
		}
		else
		{
			digitalWrite(GUITAR_PIN_WHITE, LOW);
		}

		analogWrite(SKELETON_PIN_HEAD_1_EYES, renard.channelValue(10));

		int rawHead = renard.channelValue(11);
		int headPos = 90;
		if (rawHead != 0)
		{
			headPos = map(rawHead, 0, 255, 40, 140);
		}
		//constantly writing servo values jitter fix?
		if (headPos != prevSkeletonServo1Pos)
		{
			prevSkeletonServo1Pos = headPos;
			skeleton1Servo.write(headPos);
		}

		analogWrite(SKELETON_PIN_HEAD_2_EYES, renard.channelValue(12));

		rawHead = renard.channelValue(13);
		headPos = 90;
		if (rawHead != 0)
		{
			headPos = map(rawHead, 0, 255, 40, 140);
		}
		//constantly writing servo values jitter fix?
		if (headPos != prevSkeletonServo2Pos)
		{
			prevSkeletonServo2Pos = headPos;
			skeleton2Servo.write(headPos);
		}

		if (renard.channelValue(14) >= 127)
		{
			digitalWrite(SMOKE_LEFT_PIN, HIGH);
		}
		else
		{
			digitalWrite(SMOKE_LEFT_PIN, LOW);
		}

		if (renard.channelValue(15) >= 127)
		{
			digitalWrite(SMOKE_RIGHT_PIN, HIGH);
		}
		else
		{
			digitalWrite(SMOKE_RIGHT_PIN, LOW);
		}
	}
}

//called whenever lights turn on
void randomlyPickHandLocation()
{
	if (!areWeCurrentlyMovingTheHand)
	{
		destinationPosition = random(GUITAR_HAND_MIN, GUITAR_HAND_MAX);
		if (destinationPosition != currentHandPosition)
		{
			areWeCurrentlyMovingTheHand = true;
		}
	}
}

//called ever 2 ms
void moveHand()
{
	long mil = millis();
	if (lastTimeWeMovedMotor <= mil - MILLS_BETWEEN_STEPS)
	{
		lastTimeWeMovedMotor = mil;

		if (areWeCurrentlyMovingTheHand)
		{
			if (!handIsZeroed)
			{
				int handSensor = analogRead(GUITAR_PIN_LIMIT_SENSOR);
				if (handSensor < LIMIT_SENSOR_TRIPPED)
				{
					// We have hit the zero mark.
					currentHandPosition = 0;
					handIsZeroed = true;
				}
				else
				{
					// Move down to hit the sensor.
					digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_DOWN);
				}
			}

			if (handIsZeroed)
			{
				if (currentHandPosition < destinationPosition)
				{
					digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_UP);
					currentHandPosition++;
				}
				else
				{
					digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_DOWN);
					currentHandPosition--;
				}

				if (currentHandPosition == destinationPosition)
				{
					areWeCurrentlyMovingTheHand = false;
				}
			}

			digitalWrite(GUITAR_PIN_HAND_STEP, HIGH);
			delayMicroseconds(PULSE_BETWEEN_STEPPER_STEPS); // this line is probably unnecessary
			digitalWrite(GUITAR_PIN_HAND_STEP, LOW);
		}
	}
}