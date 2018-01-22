#include <FastLED.h>

#define NUM_LEDS 12
#define DATA_PIN 2
#define MIN_VAL 40
#define HUE_CHANGE_THRESHOLD 20

class Cycler {
    public:
        enum mode_t {
            CYCLE_HUE,
            PULSE_VALUE,
            PULSE_VALUE_CHANGE_HUE,
            PULSE_VALUE_CYCLE_HUE};

    private:
        // How much to increment _value by in value/millisecond
        float _value_timebase_speed;

        // The current value
        float _value_timebase;

        // How much to increment _hue by in hue/milllisecond
        float _hue_speed;

        // Current hue
        float _hue;

        // The offset for the hue
        uint8_t _hue_offset;

        // When the last update was
        unsigned long _last_update;

        // The current mode
        mode_t _mode;

        // Speed multiplier
        float _speed_mult;

    public:
        Cycler() {
            _value_timebase_speed = 0.1;
            _value_timebase = 0;
            _hue_speed = 0.1;
            _hue = 0;
            _hue_offset = 0;
            _mode = PULSE_VALUE;
            _speed_mult = 1.0;
        }

        void init() {
            _last_update = millis();
        }

        float wrap_255_float(float input) {
            // Do a floating point modulo 0-255.999999...
            int int_part = int(input);
            float decimal_part = input - int_part;
            int wrapped = int_part % 256;
            float wrapped_float = wrapped + decimal_part;

            return wrapped_float;
        }

        void set_mode(mode_t mode) {
            _mode = mode;
        }

        uint8_t get_hue() {
            return (uint8_t(_hue) + _hue_offset) % 256;
        }

        void set_hue(float hue) {
            _hue = hue;
        }

        void set_hue_speed(float speed) {
            _hue_speed = speed;
        }

        void set_hue_offset(uint8_t hue_offset) {
            _hue_offset = hue_offset;
        }

        uint8_t get_value() {
            return sin8(uint8_t(_value_timebase));
        }

        void set_value_timebase_speed(float speed) {
            _value_timebase_speed = speed;
        }

        void set_value_timebase(float value_timebase) {
            _value_timebase = wrap_255_float(value_timebase);
        }

        void update() {
            // Get how long it's been since the last update
            unsigned long current_time = millis();
            uint8_t elapsed_millis = current_time - _last_update;

            // If measurable time has passed
            if (elapsed_millis > 0) {
                // Store this read time
                _last_update = current_time;

                // Update based on current_mode
                switch (_mode) {
                    case CYCLE_HUE:
                        update_cycle_hue(elapsed_millis);
                        break;
                    case PULSE_VALUE:
                        update_pulse_value(elapsed_millis);
                        break;
                    case PULSE_VALUE_CHANGE_HUE:
                        update_pulse_value_change_hue(elapsed_millis);
                        break;
                    case PULSE_VALUE_CYCLE_HUE:
                        update_pulse_value_change_hue(elapsed_millis);
                        break;
                }
            }
        }

        void update_cycle_hue(uint8_t elapsed_millis) {
            float new_hue = _hue + (elapsed_millis * _hue_speed * _speed_mult);
            _hue = wrap_255_float(new_hue);
        }

        void update_pulse_value(uint8_t elapsed_millis) {
            float new_value_timebase = _value_timebase + (elapsed_millis * _value_timebase_speed * _speed_mult);
            _value_timebase = wrap_255_float(new_value_timebase);
        }

        void update_pulse_value_change_hue(uint8_t elapsed_millis) {
            uint8_t _previous_value = get_value();
            update_pulse_value(elapsed_millis);
            // When it goes from above the threshold to below it
            if ((_previous_value >= HUE_CHANGE_THRESHOLD) && (get_value() < HUE_CHANGE_THRESHOLD)) {
                _hue = float(random8());
            }
        }
        void update_pulse_value_cycle_hue(uint8_t elapsed_millis) {
            update_pulse_value(elapsed_millis);
            update_cycle_hue(elapsed_millis);
        }
};

class Pixel {
    public:
        enum mode_t {
            STATIC,
            CYCLE_HUE,
            PULSE_VALUE,
            PULSE_VALUE_CHANGE_HUE
        };

    private:
        // Index of this pixel in the global array
        uint8_t _index;

        // Pointer to leds in global memory
        CRGB * _leds;

        // Current mode for the pixel
        mode_t _mode;

        // Cycler to control time based effects
        Cycler _cycler;

        // Hue, saturation and value
        uint8_t _hue, _sat, _val;

    public:
        Pixel() {
            _index = 0;
            _leds = NULL;
            _mode = STATIC;
            _hue = _sat = _val = 0;

        }

        void init() {
            _cycler.init();
        }

        void link_to_led(CRGB * leds, uint8_t index) {
            _leds = leds;
            _index = index;
        }

        void apply_to_led() {
            // Cap lowest value - it gets all crispy
            uint8_t capped_val = 0;
            if (_val < MIN_VAL) {
                capped_val = dim8_raw(MIN_VAL);
            } else {
                capped_val = dim8_raw(_val);
            }

            _leds[_index].setHSV(_hue, _sat, capped_val);
        }

        void set_hue(uint8_t hue) {
            _hue = hue;
            _cycler.set_hue(_hue);
        }

        void set_hue_speed(float hue_speed) {
            _cycler.set_hue_speed(hue_speed);
        }

        void set_hue_offset(float hue_offset) {
            _cycler.set_hue_offset(hue_offset);
        }

        void set_sat(uint8_t sat) {
            _sat = sat;
        }

        void set_val(uint8_t val) {
            _val = val;
        }

        void set_value_timebase_speed(float speed) {
            _cycler.set_value_timebase_speed(speed);
        }

        void set_value_timebase(float value_timebase) {
            _cycler.set_value_timebase(value_timebase);
        }

        void set_mode(mode_t mode) {
            _mode = mode;
            switch (_mode) {
                case STATIC:
                    set_mode_STATIC();
                    break;
                case CYCLE_HUE:
                    set_mode_CYCLE_HUE();
                    break;
                case PULSE_VALUE:
                    set_mode_PULSE_VALUE();
                    break;
                case PULSE_VALUE_CHANGE_HUE:
                    set_mode_PULSE_VALUE_CHANGE_HUE();
                    break;
            }
        }

        void set_mode_STATIC() {
            // Nothing to do!
        }

        void set_mode_CYCLE_HUE() {
            _cycler.set_mode(Cycler::CYCLE_HUE);
        }

        void set_mode_PULSE_VALUE() {
            _cycler.set_mode(Cycler::PULSE_VALUE);
        }

        void set_mode_PULSE_VALUE_CHANGE_HUE() {
            _cycler.set_mode(Cycler::PULSE_VALUE_CHANGE_HUE);
        }

        void update() {
            _cycler.update();
            switch (_mode) {
                case STATIC:
                    update_STATIC();
                    break;
                case CYCLE_HUE:
                    update_CYCLE_HUE();
                    break;
                case PULSE_VALUE:
                    update_PULSE_VALUE();
                    break;
                case PULSE_VALUE_CHANGE_HUE:
                    update_PULSE_VALUE_CHANGE_HUE();
                    break;
            }
            apply_to_led();
        }

        void update_STATIC() {
            // Nothing to do!
        }

        void update_CYCLE_HUE() {
            _hue = _cycler.get_hue();
        }

        void update_PULSE_VALUE() {
            _val = _cycler.get_value();
        }

        void update_PULSE_VALUE_CHANGE_HUE() {
            _val = _cycler.get_value();
            _hue = _cycler.get_hue();
        }
};

class PixelGroup {
    public:
        enum mode_t {
            STATIC,
            PULSE_VALUE
        };

    private:
        // Number of pixels in this group
        uint8_t _num_pixels;

        // Indecies of the pixel this group controls
        uint8_t * _pixel_indecies;

        // Pointer to pixels in global memory
        Pixel * _pixels;

        // Current mode for the group
        mode_t _mode;

        // Cycler to control time based effects
        Cycler _cycler;

    public:
        // Constructor
        PixelGroup() {
            _num_pixels = 0;
            _pixel_indecies = NULL;
            _pixels = NULL;
            _mode = STATIC;
        }

        // Initialiser
        void init(Pixel * pixels, uint8_t num_pixels, uint8_t * pixel_indecies) {
            _pixels = pixels;
            _num_pixels = num_pixels;
            // I can get away with this because the array being passed in will be global.
            _pixel_indecies = pixel_indecies;
            _mode = UNIFORM_STATIC;
            _cycler.init();
        }

        void set_hue(uint8_t hue) {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_hue(hue);
            }
        }

        void set_rainbow_hue() {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_hue((255/_num_pixels) * index);
            }
        }

        void set_sat(uint8_t sat) {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_sat(sat);
            }
        }

        void set_val(uint8_t val) {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_val(val);
            }
        }

        void set_value_timebase_speed(float speed) {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_value_timebase_speed(speed);
            }
        }

        void set_value_timebase(float timebase) {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_value_timebase(timebase);
            }
        }

        void set_mode(mode_t mode) {
            _mode = mode;
            switch (_mode) {
                case STATIC :
                    set_mode_STATIC();
                    break;
                case UNIFORM_PULSE_VALUE :
                    set_mode_PULSE_VALUE();
                    break;
            }
        }

        void set_mode_STATIC() {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_mode(Pixel::STATIC);
            }
        }

        void set_mode_PULSE_VALUE() {
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].set_mode(Pixel::PULSE_VALUE);
            }
        }

        // Update the group
        void update() {
            _cycler.update();
            switch (_mode) {
                case STATIC:
                    update_STATIC();
                    break;
                case PULSE_VALUE:
                    update_PULSE_VALUE();
                    break;
            }
        }

        void update_STATIC() {
            // Update all the pixels
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].update();
            }
        }

        void update_PULSE_VALUE() {
            // Update all the pixels
            for (int index = 0; index < _num_pixels; index++) {
                _pixels[_pixel_indecies[index]].update();
            }
        }
};

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
    ring.set_mode(PixelGroup::PULSE_VALUE);
}

void loop() {
    ring.update();
    FastLED.show();

}

