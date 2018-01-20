#include <FastLED.h>
#define NUM_LEDS 16
#define DATA_PIN 2

CRGB leds[NUM_LEDS];
int hue = 0;
int value = 255;
int divider_count = 0;
int divider = 5;

void setup() { 
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(200);
}

void loop() {

    int hue_loop = 0;
    for (int index = 0; index < NUM_LEDS; index++) {
        hue_loop = hue + ((255/NUM_LEDS) * index) % 256;
        leds[index] = CHSV(hue_loop, 255, value);
    }
    // leds[0] = CHSV(hue, 255, 128);
    hue = (hue + 1) % 256;
    // divider_count = (divider_count + 1) % (divider + 1);
    // if (divider_count == 0) {
    //     value = (value + 1) % 256;
    // }
    FastLED.show();
    delay(10);
}