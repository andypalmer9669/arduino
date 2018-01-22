#include <FastLED.h>

#define NUM_LEDS 12
#define DATA_PIN 2
#define MIN_VAL 0
#define HUE_CHANGE_THRESHOLD 20

CRGB leds[NUM_LEDS];

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
            _value_timebase_speed = 0.5;
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

Pixel pixels[12];

// Pixel p1;
// Pixel p2;

void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(32);

    for (int index = 0; index < NUM_LEDS; index++) {
        pixels[index].init();
        pixels[index].link_to_led(leds, index);
        pixels[index].set_hue(128);
        pixels[index].set_sat(255);
        pixels[index].set_val(255);
        pixels[index].set_mode(Pixel::PULSE_VALUE);
        pixels[index].set_value_timebase_speed(0.1);
    }

    // p1.init();
    // p1.link_to_led(leds, 0);
    // p1.set_hue(128);
    // p1.set_sat(255);
    // p1.set_val(255);
    // p1.set_mode(Pixel::PULSE_VALUE_CHANGE_HUE);
    // p1.set_value_timebase_speed(0.1);

    // p2.init();
    // p2.link_to_led(leds, 1);
    // p2.set_hue(0);
    // p2.set_sat(255);
    // p2.set_val(255);
    // p2.set_mode(Pixel::PULSE_VALUE);
    // p2.set_value_timebase_speed(0.1);
    // p2.set_value_timebase(128);
}

void loop() {
    for (int index = 0; index < NUM_LEDS; index++) {
        pixels[index].update();
    }
    // p1.update();
    // p2.update();
    FastLED.show();
}
