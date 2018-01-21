#include <FastLED.h>

// Could define speed as number of millis per hue
// and keep a count rather than a floating point

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
        _value_timebase_speed = 1;
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
        float result = 0.0;

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

    uint8_t get_value() {
        return sin8(uint8_t(_value_timebase));
    }

    void set_hue(float hue) {
        _hue = hue;
    }

    void set_value_timebase(float value_timebase) {
        _value_timebase = wrap_255_float(value_timebase);
    }

    void set_hue_offset(uint8_t hue_offset) {
        _hue_offset = hue_offset;
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

Cycler c0;
Cycler c1;
Cycler c2;
Cycler c3;
Cycler c4;
// Cycler c5;
// Cycler c6;
// Cycler c7;
// Cycler c8;
// Cycler c9;

// float nums[10] = {0,1,2,3,4,5,6,7,8,9};

void setup() {
    // put your setup code here, to run once:
    c0.init();
    c1.init();
    c2.init();
    c3.init();
    c4.init();
    Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    c0.update();
    c1.update();
    c2.update();
    c3.update();
    c4.update();
    // c5.update();
    // c6.update();
    // c7.update();
    // c8.update();
    // c9.update();
    // c8.update();
    Serial.println(c0.get_value());
}













