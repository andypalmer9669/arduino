#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 3

enum mode {SOLID};

class PixelGroup {
private:
	// Number of leds in this group
	int _num_leds;

	// Indecies of the leds this group controls
	int led_indecies[];

	// Pointer to pixels in global memory
	CRGB * _leds;

	// How long to wait between updates
	unsigned long _interval;

	// When the last update was
	unsigned long _last_update;

public:
	// Constructor
	PixelGroup(CRGB * leds, int num_leds, int * led_indecies, unsigned long interval);

	// Take the in the group and apply it to the pixels in memory
	void apply_to_leds(CRGB leds[]);

	// Update the group
	bool update();
};

PixelGroup::PixelGroup(CRGB * leds, int num_leds, int * led_indecies, unsigned long interval) {
	_leds = leds;
	_led_indecies = led_indecies;
	_num_leds = num_leds;
	_interval = interval;
	_last_update = 0;
};

bool PixelGroup::update() {
	bool updated = false;
 	unsigned long current_millis = millis();
 	if (current_millis - _last_update > _interval) {
 		// Do updatey things
 		updated = true;
 		_last_update = current_millis;
 	}
 	return updated

}

CRGB leds[NUM_LEDS];

int led_indecies[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
unsigned long interval = 5;

PixelGroup ring(leds, 16, led_indecies, interval);

void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
	ring.update();
	FastLED.show();

}

