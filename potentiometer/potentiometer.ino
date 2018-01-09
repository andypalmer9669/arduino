class Potentiometer {
    // To use pots, connect the outer pins of the pot to ground and +5v, then the 
    // middle pin to the analog input pin chosen.

    private:
        // The pin this potentiometer is reading from to
        uint8_t _pin;
  
        // The value of the potentiometer
        uint16_t _value;
    
        // How many millisecods to wait between updates
        uint8_t _update_period;

        // When the last update was
        unsigned long _last_update;
    
        // Callback for when the value changes
        void (*_callback)(uint16_t);
  
    public:
        // Constructor
        Potentiometer(uint8_t pin, uint8_t update_period, void (*callback)(uint16_t)) {
            _pin = pin;
            _update_period = update_period;
            _callback = callback;
            _last_update = 0;
        }
    
        // Class setup
        void init() {
            pinMode(_pin, INPUT);
            _value = analogRead(_pin);
        }
    
        // The most up to date value of the potentiometer
        uint16_t get_value() {
            return _value;
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
                uint16_t test_val = analogRead(_pin);


                // Getting really weird flickering of values for vals > ~970
                signed char diff = test_val - _value;
                if (abs(diff) > 2) {
                    // Store it
                    _value = test_val;

                    // Call the callback
                    _callback(_value);
                }

                // If the value has changed by more than one
                // if (test_val != _value) {
                //     // Store it
                //     _value = test_val;

                //     // Call the callback
                //     _callback(_value);
                // }
            }
        }
};

void print_change(uint16_t new_val){
    Serial.println(new_val);
}

Potentiometer pot(A1, 10, &print_change);

void setup() {
    // put your setup code here, to run once:
    pot.init();
    Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
     pot.update();
}
