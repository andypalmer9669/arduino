#include <FastLED.h>

#define NUM_LEDS 22
#define DATA_PIN 2

CRGB leds[NUM_LEDS];

void setup() {
    // put your setup code here, to run once:
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(192);

}

void loop() {
    // put your main code here, to run repeatedly:
    for(int index = 0; index < NUM_LEDS; index++) { 
        leds[index] = CRGB::Red;
        FastLED.show();
        // clear this led for the next time around the loop
        leds[index] = CRGB::Black;
        delay(1000);
    }
}