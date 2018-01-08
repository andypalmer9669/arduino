#include <FastLED.h>

// http://gamesfromwithin.com/stupid-c-tricks-2-better-enums
// namespace LinearLedState
// {
//     enum Enum
//     {
//         Invalid,
//         Run,
//         Exit,
//         Restart,
//         Restore,
//     };
// }


#define NUM_LEDS 4
#define DATA_PIN 2

enum mode_t {CONSTANT, PULSE_VALUE_LINEAR, PULSE_VALUE_SIN, };

class LED {
    private:
        float _hue;
        float _saturation;
        float _value;
        float _max_value;
        float _min_value;

        unsigned short _pulse_value_linear_offset;
        unsigned short _pulse_value_linear_period;

        uint32_t _pulse_value_sin_offset;
        uint8_t _pulse_value_sin_bpm;

        unsigned char _index;
        CRGB * _leds;
        mode_t _mode;

    public:
        LED() {
            init();
        }

        // Constructor
        LED(CRGB * leds, int index) {
            init();
            _leds = leds;
            _index = index;
        }

        void init() {
            _leds = NULL;
            _index = 0;
            _mode = PULSE_VALUE_SIN;

            _hue = 128;
            _saturation = 255;
            _value = 255;
            _max_value = 255;
            _min_value = 40;

            _pulse_value_linear_offset = 0;
            _pulse_value_linear_period = 1000;

            _pulse_value_sin_offset = 0;
            _pulse_value_sin_bpm = 60;

        }

        void link_to_globals(CRGB * leds, int index) {
            _leds = leds;
            _index = index;
        }

        void set_hue(float hue) {
            _hue = hue;
        }

        // Set the value of the pixel
        void set_value(float value) {
            // if (value > _max_value) {
            //     _value = _max_value;
            // } else if (value < _min_value) {
            //     _value = _min_value;
            // } else {
            //     _value = value;
            // }

            if (value > _max_value) {
                _value = dim8_raw(_max_value);
            } else if (value < _min_value) {
                _value = dim8_raw(_min_value);
            } else {
                _value = dim8_raw(value);
            }
        }

        void set_pulse_value_linear_offset(unsigned short offset) {
            _pulse_value_linear_offset = offset;
        }

        void set_pulse_value_linear_period(unsigned short period) {
            _pulse_value_linear_period = period;
        }

        void set_pulse_value_sin_bpm(uint8_t bpm) {
            _pulse_value_sin_bpm = bpm;
        }

        void set_pulse_value_sin_offset(uint32_t offset) {
            _pulse_value_sin_offset = offset;
        }

        // Top level update
        void update() {
            switch(_mode){
                case CONSTANT:
                    update_constant();
                    break;
                case PULSE_VALUE_LINEAR:
                    update_pulse_value_linear();
                    break;
                case PULSE_VALUE_SIN:
                    update_pulse_value_sin();
                    break;
                default:
                    break;
            }
            _leds[_index].setHSV(_hue, _saturation, _value);
        }

        // Update when in constant mode
        void update_constant() {
            // Nothing to do!
        }

        // Update when in pulse mode
        void update_pulse_value_linear() {
            // Convert the current time to a time in the current period
            float period_time = (millis() + _pulse_value_linear_offset) % _pulse_value_linear_period;

            // Find the middle of this time period
            float mid_time = float(_pulse_value_linear_period) / 2.0;

            // If we should be rising
            if (period_time < mid_time) {
                // Find the percentage between start of period and mid
                float scale = period_time / mid_time;

                // Use percentage as a scale value
                set_value(_min_value + (scale * (_max_value - _min_value)));
            // Else we're falling
            } else {
                // Find the percentage between middle of period and end
                float scale = (period_time - mid_time) / mid_time;

                // Use percentage as a scale value
                set_value(_max_value - (scale * (_max_value - _min_value)));
            }
        }

        void update_pulse_value_sin() {
            set_value(beatsin8(
                _pulse_value_sin_bpm,
                _min_value,
                _max_value,
                _pulse_value_sin_offset
            ));
        }
};

CRGB global_leds[NUM_LEDS];
LED leds[NUM_LEDS];

// LED red, green, blue;


void setup() {
    // put your setup code here, to run once:
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(global_leds, NUM_LEDS);
     FastLED.setBrightness(128);

    // indiv RGB
    // red.link_to_globals(global_leds, 0);
    // red.set_hue(0);
    // red.set_pulse_value_period(3000);
    // red.set_pulse_value_offset(0);

    // green.link_to_globals(global_leds, 5);
    // green.set_hue(96);
    // green.set_pulse_value_period(3000);
    // green.set_pulse_value_offset(1000);

    // blue.link_to_globals(global_leds, 10);
    // blue.set_hue(160);
    // blue.set_pulse_value_period(3000);
    // blue.set_pulse_value_offset(2000);



    // Sin fade
    for (int index = 0; index < NUM_LEDS; index++) {
        leds[index].link_to_globals(global_leds, index);
        leds[index].set_pulse_value_sin_bpm(15);
        leds[index].set_pulse_value_sin_offset(1000*index);
    }


    // Rotating fade
    // for (int index = 0; index < NUM_LEDS; index++) {
    //     leds[index].link_to_globals(global_leds, index);
    //     leds[index].set_pulse_value_period(12000);
    //     leds[index].set_pulse_value_offset((12000/NUM_LEDS) * index);
    // }


    // Random rainbow
    // fill_rainbow(global_leds, NUM_LEDS, 0, 15);
    // for (int index = 0; index < NUM_LEDS; index++) {
    //     leds[index].set_hue(random8());
    //     leds[index].link_to_globals(global_leds, index);
    //     leds[index].set_pulse_value_period(random16(1000, 6000));
    //     leds[index].set_pulse_value_offset(random16(1000));
    //}



    // Print dimming behaviour
    // Serial.begin(9600);
    // for (int index = 0; index < 256; index++) {
    //     Serial.println(index);
    //     Serial.println(dim8_raw(index));
    //     Serial.println(brighten8_raw(index));
    //     Serial.println("---");
    // }
}

void loop() {
    // put your main code here, to run repeatedly:
    for (int index = 0; index < NUM_LEDS; index++) {
        leds[index].update();
    }

    // red.update();
    // blue.update();
    // green.update();

    FastLED.show();

    // Serial.println(millis() - last_update);
    // last_update = millis();
}




