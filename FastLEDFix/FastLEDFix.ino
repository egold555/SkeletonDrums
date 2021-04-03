#include <FastLED.h>

#define DRUM_1_PIN 2
#define DRUM_2_PIN 3
#define DRUM_3_PIN 4
#define DRUM_4_PIN 5
#define DRUM_5_PIN 6

#define DRUM_1_PIN_IN A2
#define DRUM_2_PIN_IN A3
#define DRUM_3_PIN_IN A4
#define DRUM_4_PIN_IN A5

//People @ AVR are fucking stupid. A0-A5 can be used to digitial read, A6 and A7 can't!
//This took us fucking hours to solve, pulling our hair out.
#define DRUM_5_PIN_IN A0

#define DRUM_1_LED_COUNT 44
#define DRUM_2_LED_COUNT 36
#define DRUM_3_LED_COUNT 46
#define DRUM_4_LED_COUNT 55
#define DRUM_5_LED_COUNT 73

const long decayTime = 250; // TIme for drums to decay in ms.

CRGB drum1Leds[DRUM_1_LED_COUNT];
CRGB drum2Leds[DRUM_2_LED_COUNT];
CRGB drum3Leds[DRUM_3_LED_COUNT];
CRGB drum4Leds[DRUM_4_LED_COUNT];
CRGB drum5Leds[DRUM_5_LED_COUNT];

long lastTimeHigh1 = 0, lastTimeHigh2 = 0, lastTimeHigh3 = 0, lastTimeHigh4 = 0, lastTimeHigh5 = 0;

void setup()
{
	Serial.begin(9600);
	pinMode(DRUM_1_PIN_IN, INPUT);
	pinMode(DRUM_2_PIN_IN, INPUT);
	pinMode(DRUM_3_PIN_IN, INPUT);
	pinMode(DRUM_4_PIN_IN, INPUT);
	pinMode(DRUM_5_PIN_IN, INPUT);

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
}
void loop()
{
	int brightness = 0;

	brightness = read(DRUM_1_PIN_IN, &lastTimeHigh1);
	fill_solid(drum1Leds, DRUM_1_LED_COUNT, CRGB(brightness, 0, 0));

	brightness = read(DRUM_2_PIN_IN, &lastTimeHigh2);
	fill_solid(drum2Leds, DRUM_2_LED_COUNT, CRGB(0, brightness, 0));

	brightness = read(DRUM_3_PIN_IN, &lastTimeHigh3);
	fill_solid(drum3Leds, DRUM_3_LED_COUNT, CRGB(0, 0, brightness));

	brightness = read(DRUM_4_PIN_IN, &lastTimeHigh4);
	fill_solid(drum4Leds, DRUM_4_LED_COUNT, CRGB(brightness, brightness, 0));

	brightness = read(DRUM_5_PIN_IN, &lastTimeHigh5);
	fill_solid(drum5Leds, DRUM_5_LED_COUNT, CRGB(brightness, 0, brightness));

	// Serial.print(analogRead(DRUM_1_PIN_IN));
	// Serial.print(", ");
	// Serial.print(analogRead(DRUM_2_PIN_IN));
	// Serial.print(", ");
	// Serial.print(analogRead(DRUM_3_PIN_IN));
	// Serial.print(", ");
	// Serial.print(analogRead(DRUM_4_PIN_IN));
	// Serial.print(", ");
	// Serial.print(analogRead(DRUM_5_PIN_IN));
	// Serial.println();

	// Serial.print(digitalRead(DRUM_1_PIN_IN));
	// Serial.print(", ");
	// Serial.print(digitalRead(DRUM_2_PIN_IN));
	// Serial.print(", ");
	// Serial.print(digitalRead(DRUM_3_PIN_IN));
	// Serial.print(", ");
	// Serial.print(digitalRead(DRUM_4_PIN_IN));
	// Serial.print(", ");
	// Serial.print(digitalRead(DRUM_5_PIN_IN));
	// Serial.println();

	FastLED.show();
}

int read(int inputPin, long *lastTimeHigh)
{
	long currentTime = millis();

	if (digitalRead(inputPin) == HIGH)
	{
		*lastTimeHigh = currentTime;
		return 255;
	}
	else
	{
		long timeLow = currentTime - *lastTimeHigh;
		if (timeLow >= decayTime)
		{
			return 0;
		}
		else
		{
			return map(timeLow, 0, decayTime, 255, 0);
		}
	}
}