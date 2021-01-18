#include <FastLED.h>
#include <Renard.h>
//https://learn.adafruit.com/neopixels-and-servos/the-ticoservo-library
#include "Adafruit_TiCoServo.h"

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

#define DRUM_1_LED_COUNT 44
#define DRUM_2_LED_COUNT 36
#define DRUM_3_LED_COUNT 46
#define DRUM_4_LED_COUNT 55
#define DRUM_5_LED_COUNT 73

#define GUITAR_PIN_HAND_STEP 50
#define GUITAR_PIN_HAND_DIRECTION 52

#define GUITAR_HAND_MIN 0
#define GUITAR_HAND_MAX 800
int currentHandPosition = GUITAR_HAND_MIN;
int destinationPosition = currentHandPosition;
bool areWeCurrentlyMovingTheHand = false;
#define HAND_UP LOW
#define HAND_DOWN HIGH
#define PULSE_BETWEEN_STEPPER_STEPS 20
#define MILLS_BETWEEN_STEPS 2
long lastTimeWeMovedMotor = 0;

CRGB drum1Leds[DRUM_1_LED_COUNT];
CRGB drum2Leds[DRUM_2_LED_COUNT];
CRGB drum3Leds[DRUM_3_LED_COUNT];
CRGB drum4Leds[DRUM_4_LED_COUNT];
CRGB drum5Leds[DRUM_5_LED_COUNT]; 

Adafruit_TiCoServo skeleton1Servo;
Adafruit_TiCoServo skeleton2Servo;

Renard renard(Serial, 15);

int prevSkeletonServo1Pos = 90;
int prevSkeletonServo2Pos = 90;

void setup()
{

  Serial.begin(57600);

  FastLED.addLeds<WS2811, DRUM_1_PIN, GRB>(drum1Leds, DRUM_1_LED_COUNT);
  FastLED.addLeds<WS2811, DRUM_2_PIN, GRB>(drum2Leds, DRUM_2_LED_COUNT);
  FastLED.addLeds<WS2811, DRUM_3_PIN, GRB>(drum3Leds, DRUM_3_LED_COUNT);
  FastLED.addLeds<WS2811, DRUM_4_PIN, GRB>(drum4Leds, DRUM_4_LED_COUNT);
  FastLED.addLeds<WS2811, DRUM_5_PIN, GRB>(drum5Leds, DRUM_5_LED_COUNT);
  
  FastLED.setBrightness(100);
  fill_solid(drum1Leds, DRUM_1_LED_COUNT, CRGB::Black);
  fill_solid(drum2Leds, DRUM_2_LED_COUNT, CRGB::Black);
  fill_solid(drum3Leds, DRUM_3_LED_COUNT, CRGB::Black);
  fill_solid(drum4Leds, DRUM_4_LED_COUNT, CRGB::Black);
  fill_solid(drum5Leds, DRUM_5_LED_COUNT, CRGB::Black);
  FastLED.show();

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
  if(renard.processInput()) {

    int brightness = 0;

    brightness = renard.channelValue(1);
    fill_solid(drum1Leds, DRUM_1_LED_COUNT, CRGB(brightness, 0, 0));

    brightness = renard.channelValue(2);
    fill_solid(drum2Leds, DRUM_2_LED_COUNT, CRGB(0, brightness, 0));

    brightness = renard.channelValue(3);
    fill_solid(drum3Leds, DRUM_3_LED_COUNT, CRGB(0, 0, brightness));

    brightness = renard.channelValue(4);
    fill_solid(drum4Leds, DRUM_4_LED_COUNT, CRGB(brightness, brightness, 0));

    brightness = renard.channelValue(5);
    fill_solid(drum5Leds, DRUM_5_LED_COUNT, CRGB(brightness, brightness, brightness));
    
    if(renard.channelValue(6) >= 127) {
        digitalWrite(GUITAR_PIN_RED, HIGH);
        randomlyPickHandLocation();
    }
    else {
      digitalWrite(GUITAR_PIN_RED, LOW);
    }

    if(renard.channelValue(7) >= 127) {
        digitalWrite(GUITAR_PIN_BLUE, HIGH);
        randomlyPickHandLocation();
    }
    else {
      digitalWrite(GUITAR_PIN_BLUE, LOW);
    }

    if(renard.channelValue(8) >= 127) {
        digitalWrite(GUITAR_PIN_GREEN, HIGH);
        randomlyPickHandLocation();
    }
    else {
      digitalWrite(GUITAR_PIN_GREEN, LOW);
    }

    if(renard.channelValue(9) >= 127) {
        digitalWrite(GUITAR_PIN_WHITE, HIGH);
        randomlyPickHandLocation();
    }
    else {
      digitalWrite(GUITAR_PIN_WHITE, LOW);
    }
    
    analogWrite(SKELETON_PIN_HEAD_1_EYES, renard.channelValue(10));

    int rawHead = renard.channelValue(11);
    int headPos = 90;
    if(rawHead != 0) {
      headPos = map(rawHead, 0, 255, 0, 180);
    }
    //constantly writing servo values jitter fix?
    if(headPos != prevSkeletonServo1Pos) {
      prevSkeletonServo1Pos = headPos;
      skeleton1Servo.write(headPos);
      
    }


    analogWrite(SKELETON_PIN_HEAD_2_EYES, renard.channelValue(12));

    rawHead = renard.channelValue(13);
    headPos = 90;
    if(rawHead != 0) {
      headPos = map(rawHead, 0, 255, 0, 180);
    }
    //constantly writing servo values jitter fix?
    if(headPos != prevSkeletonServo2Pos) {
      prevSkeletonServo2Pos = headPos;
      skeleton2Servo.write(headPos);
      
    }

    if(renard.channelValue(14) >= 127) {
        digitalWrite(SMOKE_LEFT_PIN, HIGH);
    }
    else {
      digitalWrite(SMOKE_LEFT_PIN, LOW);
    }

    if(renard.channelValue(15) >= 127) {
        digitalWrite(SMOKE_RIGHT_PIN, HIGH);
    }
    else {
      digitalWrite(SMOKE_RIGHT_PIN, LOW);
    }
    
    FastLED.show();
  }

}

//called whenever lights turn on
void randomlyPickHandLocation() {
    if(!areWeCurrentlyMovingTheHand) {
      destinationPosition = random(GUITAR_HAND_MIN, GUITAR_HAND_MAX);
      if(destinationPosition != currentHandPosition) {
        areWeCurrentlyMovingTheHand = true;
      }
    }
}


//called ever 2 ms
void moveHand() {
    long mil = millis();
    if(lastTimeWeMovedMotor <= mil - MILLS_BETWEEN_STEPS) {
        lastTimeWeMovedMotor = mil;

        if(areWeCurrentlyMovingTheHand) {

            if(currentHandPosition < destinationPosition) {
              digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_UP);
              currentHandPosition++;
            }
            else {
              digitalWrite(GUITAR_PIN_HAND_DIRECTION, HAND_DOWN);
              currentHandPosition--;
            }

            if(currentHandPosition == destinationPosition) {
                areWeCurrentlyMovingTheHand = false;
            }

            digitalWrite(GUITAR_PIN_HAND_STEP, HIGH);
            delayMicroseconds(PULSE_BETWEEN_STEPPER_STEPS); // this line is probably unnecessary
            digitalWrite(GUITAR_PIN_HAND_STEP, LOW);

        }
    }

}