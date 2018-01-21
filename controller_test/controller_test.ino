#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define BUTTON1_PIN 3
#define BUTTON2_PIN 4

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
        void (*_callback)(uint16_t);

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


void button_1_press(){
    Serial.println("Button 1 Pressed");
    Serial.println(millis());
}

void button_2_press(){
    Serial.println("Button 2 Pressed");
    Serial.println(millis());
}

void pot_1_change(uint8_t new_val){
    Serial.println("pot1");
    Serial.println(new_val);
}

void pot_2_change(uint8_t new_val){
    Serial.println("pot2");
    Serial.println(new_val);
}

void pot_3_change(uint8_t new_val){
    Serial.println("pot3");
    Serial.println(new_val);
}

Potentiometer pot1(POT1_PIN, 10, &pot_1_change);
Potentiometer pot2(POT2_PIN, 10, &pot_2_change);
Potentiometer pot3(POT3_PIN, 10, &pot_3_change);
Button button1(BUTTON1_PIN, 50, &button_1_press);
Button button2(BUTTON2_PIN, 50, &button_2_press);

void setup() {
    // put your setup code here, to run once:
    pot1.init();
    pot2.init();
    pot3.init();
    button1.init();
    button2.init();
    Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    pot1.update();
    pot2.update();
    pot3.update();
    button1.update();
    button2.update();
}
