#include <FastLED.h>

#define NUM_LEDS 12
#define DATA_PIN 2

class Display {
    public:
        enum mode_t {
            UNIFORM_STATIC,
            UNIFORM_PULSE_VALUE,
            VARIED_STATIC,
            VARIED_PULSE_VALUE
        };

    private:
        Button _mode_button;
        Button _function_button;

        Potentiometer _pot_A;
        Potentiometer _pot_B;
        Potentiometer _pot_C;

        PixelGroup _grp_all_pixels;
        PixelGroup _grp_ring;
        PixelGroup _grp_not_ring;

    public:
        Display() {

        }

        void init(
            Pixel * pixels,
            uint8_t * _all_pixel_indecies,
            uint8_t * _ring_indecies,
            uint8_t * _not_ring_indecies) {

        }
}

CRGB leds[NUM_LEDS];
Pixel pixels[NUM_LEDS];
uint8_t ring_pixel_indecies[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
PixelGroup ring;

void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(128);

    // Pixel setup
    for (int index = 0; index < NUM_LEDS; index++) {
        pixels[index].init();
        pixels[index].link_to_led(leds, index);
    }

    // Ring setup
    ring.init(pixels, 12, ring_pixel_indecies);
    // ring.set_hue(255);
    ring.set_rainbow_hue();
    ring.set_sat(255);
    ring.set_val(255);
    ring.set_mode(PixelGroup::UNIFORM_PULSE_VALUE);
}

void loop() {
    ring.update();
    FastLED.show();

}