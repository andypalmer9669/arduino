class Button
{
    private:
        // The pin this button is connected to
        int _pin;
  
        // The time in milliseconds for the state to be held before considering it to be on
        unsigned long _debounce_time;

        // Whether the button is pressed or not
        bool _is_pressed;
    
        // The state of the pin read at the last update
        bool _last_read;
    
        // When the button last transitioned from low to high (without bouncing)
        unsigned long _last_activation;

        // Callback for when the button is pressed
        void (*_on_complete)();
  
    public:
        // Constructor
        Button(int pin, int debounce_time, void (*callback)());
    
        // Class setup
        void init();
    
        // Determine if the button is pressed or not
        bool is_pressed();
    
        // Update the button each time round the main loop
        void update();
};

Button::Button(int pin, int debounce_time, void (*callback)())
{
    _pin = pin;
    _debounce_time = debounce_time;
    _is_pressed = false;
    _last_read = LOW;
    _last_activation = 0;
    _on_complete = callback;
}

void Button::init()
{
    pinMode(_pin, INPUT);
}

bool Button::is_pressed()
{
    return _is_pressed;
}

void Button::update()
{
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
                    if (_on_complete != NULL) {
                        _on_complete();
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

void blah(){
    Serial.println("Button Pressed");
    Serial.println(millis());
}

int BUTTON_PIN = 2;
Button button(BUTTON_PIN, 50, &blah);

void setup() {
    // put your setup code here, to run once:
    button.init();
    Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
     button.update();
}
