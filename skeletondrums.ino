#include <FastLED.h>
#include <Renard.h>

#define DRUM_1_PIN 2
#define DRUM_2_PIN 3
#define DRUM_3_PIN 4
#define DRUM_4_PIN 5
#define DRUM_5_PIN 6

#define GUITAR_PIN_RED 14
#define GUITAR_PIN_BLUE 15
#define GUITAR_PIN_GREEN 16
#define GUITAR_PIN_WHITE 17

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

Renard renard(Serial, 9);

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
  lastTimeWeMovedMotor = millis();
}
void loop()
{
  moveHand();
  if(renard.processInput()) {

    // int drumR = 0;
    // int drumG = 0;
    // int drumB = 0;

    // drumR = renard.channelValue(1);
    // drumG = renard.channelValue(2);
    // drumB = renard.channelValue(3);
    // fill_solid(drum1Leds, DRUM_1_LED_COUNT, CRGB(drumR, drumG, drumB));

    // drumR = renard.channelValue(4);
    // drumG = renard.channelValue(5);
    // drumB = renard.channelValue(6);
    // fill_solid(drum2Leds, DRUM_2_LED_COUNT, CRGB(drumR, drumG, drumB));

    // drumR = renard.channelValue(7);
    // drumG = renard.channelValue(8);
    // drumB = renard.channelValue(9);
    // fill_solid(drum3Leds, DRUM_3_LED_COUNT, CRGB(drumR, drumG, drumB));

    // drumR = renard.channelValue(10);
    // drumG = renard.channelValue(11);
    // drumB = renard.channelValue(12);
    // fill_solid(drum4Leds, DRUM_4_LED_COUNT, CRGB(drumR, drumG, drumB));

    // drumR = renard.channelValue(13);
    // drumG = renard.channelValue(14);
    // drumB = renard.channelValue(15);
    // fill_solid(drum5Leds, DRUM_5_LED_COUNT, CRGB(drumR, drumG, drumB));

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
    
    
    FastLED.show();
  }
/*
  fill_solid(drum1Leds, DRUM_1_LED_COUNT, CRGB(255, 255, 255));
  fill_solid(drum2Leds, DRUM_2_LED_COUNT, CRGB(255, 0, 0));
  fill_solid(drum3Leds, DRUM_3_LED_COUNT, CRGB(0, 255, 0));
  fill_solid(drum4Leds, DRUM_4_LED_COUNT, CRGB(0, 0, 255));
  fill_solid(drum5Leds, DRUM_5_LED_COUNT, CRGB(255, 0, 255));
  FastLED.show();
*/
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