#include <FastLED.h>

#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define BUTTON1_PIN 3
#define BUTTON2_PIN 4

#define MIN_VAL 40
#define HUE_CHANGE_THRESHOLD 20

#define NUM_LEDS 22
#define DATA_PIN 2

enum mode_t {STATIC};

class Button {
    private:
        // The pin this button is connected to
        uint8_t _pin;
  
        // The time in milliseconds for the state to be held before considering it to be on
        uint8_t _debounce_time;

        // Whether the button is pressed or not
        bool _is_pressed;
    
        // The state of the pin read at the last update
        bool _last_read;
    
        // When the button last transitioned from low to high (without bouncing)
        unsigned long _last_activation;

        // Callback for when the button is pressed
        void (*_callback)();
  
    public:
        // Constructor
        Button(uint8_t pin, uint8_t debounce_time, void (*callback)()) {
            _pin = pin;
            _debounce_time = debounce_time;
            _is_pressed = false;
            _last_read = LOW;
            _last_activation = 0;
            _callback = callback;
        }
    
        // Class setup
        void init() {
            pinMode(_pin, INPUT);
        }
    
        // Determine if the button is pressed or not
        bool is_pressed() {
            return _is_pressed;
        }
    
        // Update the button each time round the main loop
        void update() {
            // Read state of the button
            int current_read = digitalRead(_pin);

            // If the button is on
            if (current_read == HIGH) {

                // If the button isn't already activated
                if (_is_pressed == false) {
              
                    // Get current time
                    unsigned long current_time = millis();
          
                    // If it was on before
                    if (_last_read == HIGH) {
              
                        // If it's been on for longer that the debounce time set state to on, otherwise just keep waiting
                        if (current_time - _last_activation > _debounce_time) {
                            // The button is on - set the state to on and call the callback
                            _is_pressed = true;
                            if (_callback != NULL) {
                                _callback();
                            }
                        }
                    // Else it's just turned on, track when it turned on, and that it's on    
                    } else {
                        _last_activation = current_time;
                        _last_read = HIGH;
                    }
                }
            // Else the button is off. reset counters and set state to off
            } else {
                _is_pressed = false;
                _last_read = LOW;
            }
        }
};

class Potentiometer {
    // To use pots, connect the outer pins of the pot to ground and +5v, then the 
    // middle pin to the analog input pin chosen.

    private:
        // The pin this potentiometer is reading from
        uint8_t _pin;

        // The remapped value of the potentiometer
        uint8_t _value;

        // How many millisecods to wait between updates
        uint8_t _update_period;

        // When the last update was
        unsigned long _last_update;

        // Callback for when the value changes
        void (*_callback)(uint8_t);

    public:
        // Constructor
        Potentiometer(uint8_t pin, uint8_t update_period, void (*callback)(uint8_t)) {
            _pin = pin;
            _update_period = update_period;
            _callback = callback;
            _last_update = 0;
        }

        // Class setup
        void init() {
            pinMode(_pin, INPUT);
            _value = de_log_and_map(analogRead(_pin));
        }

        // The most up to date value of the potentiometer
        uint8_t get_value() {
            return _value;
        }

        // Need to do this de-logging because I bought logarithmic rather than 
        // linear potentiometers :(
        uint8_t de_log_and_map(uint16_t input) {
            uint8_t result = 0;
            if (input <= 150) {
                result = map(input, 0, 150, 0, 128);
            } else {
                result = map(input, 151, 1023, 129, 255);
            }
            return result;
        }

        // Update the potentiometer each time round the main loop
        void update() {
            // Get current time
            unsigned long current_time = millis();

            // If it's time to re-read the pot
            if (current_time - _last_update > _update_period) {
                // Store this read time
                _last_update = current_time;

                // Get the current value
                uint8_t test_val = de_log_and_map(analogRead(_pin));

                // Need to do this because the output is really noisy :(
                signed char diff = test_val - _value;
                if (abs(diff) > 2) {
                    // Store it
                    _value = test_val;

                    // Call the callback
                    if (_callback != NULL) {
                        _callback(_value);
                    }
                }
            }
        }
};

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

Potentiometer pot1;
Potentiometer pot2;
Potentiometer pot3;
Button button1;
Button button2;
PixelGroup all_pixels;
PixelGroup ring_pixels;
PixelGroup not_ring_pixels;
mode_t mode;

void pot_1_change(uint8_t new_val) {

}

void pot_2_change(uint8_t new_val) {

}

void pot_3_change(uint8_t new_val) {

}

void button_1_press() {

}

void button_2_press() {

}

void update() {

}

void update_STATIC() {

}

void set_mode(mode_t mode) {

}

void set_mode_STATIC() {

}

CRGB leds[NUM_LEDS];
Pixel pixels[NUM_LEDS];

pot1 = Potentiometer(POT1_PIN, 10, &pot_1_change);
pot2 = Potentiometer(POT2_PIN, 10, &pot_2_change);
pot3 = Potentiometer(POT3_PIN, 10, &pot_3_change);
button1 = Button(BUTTON1_PIN, 50, &button_1_press);
button2 = Button(BUTTON2_PIN, 50, &button_2_press);

PixelGroup all_pixels();
uint8_t all_pixel_indecies[22] = {0,1,2,3,4, 5,6,7,8,9, 10,11,12,13,14, 15,16,17,18,19 ,20,21};

PixelGroup ring_pixels();
uint8_t ring_pixel_indecies[16] = {6,7,8,9,10, 11,12,13,14,15, 16,17,18,19,20, 21};

PixelGroup not_ring_pixels();
uint8_t ring_pixel_indecies[6] = {0,1,2,3,4, 5};

void setup() {
    // put your setup code here, to run once:

    // LED setup
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(128);

    // Pixel setup
    for (int index = 0; index < NUM_LEDS; index++) {
        pixels[index].init();
        pixels[index].link_to_led(leds, index);
    }

    // Group Setup
	all_pixels.init(pixels, 22, all_pixel_indecies);
	ring_pixels.init(pixels, 16, ring_pixel_indecies);
	not_ring_pixels.init(pixels, 6, not_ring_pixel_indecies);

	// Input Setup
    pot1.init();
    pot2.init();
    pot3.init();
    button1.init();
    button2.init();

    // State setup
    mode = STATIC;
    set_mode(mode);
}

void loop() {
    // put your main code here, to run repeatedly:
	update();
	FastLED.show();

}