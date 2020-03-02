// Convenience class to use a button with Arduino
// 
// To use a button, have it ground the pin it's connected to when set to "on".

#include "button.h"

Button::Button() {
    constructor_defaults();
}

// Constructor
//
// pin_ - The pin the button is connected to
// debounce_time_ - The debounce time for the button.
Button::Button(byte pin_, byte debounce_time_) {
    constructor_defaults();
    set_pin(pin_);
    set_debounce_time(debounce_time_);
}

void Button::constructor_defaults() {
    pin = 0;
    debounce_time = 10;
    stable_state = LOW;
    last_read_state = LOW;
    last_state_change = millis();
    repeat_delay = 1000;
    repeat_frequency = 333;
    repeating = false;
    last_repeat_time = millis();
}

void Button::set_pin(byte pin_){
    pin = pin_;
}

void Button::set_debounce_time(byte debounce_time_){
    debounce_time = debounce_time_;
}

void Button::set_repeat_delay(unsigned long repeat_delay_){
    repeat_delay = repeat_delay_;
}

void Button::set_repeat_frequency(unsigned long repeat_frequency_){
    repeat_frequency = repeat_frequency_;
}

byte Button::get_state(){
    return stable_state;
}

// Set up button ready for use.
// Note that we're using INPUT_PULLUP to save on having to add a resistor
void Button::init() {
    pinMode(pin, INPUT_PULLUP);
    stable_state, last_read_state = !digitalRead(pin);
    last_state_change = millis();
    last_repeat_time = millis();
}

// Update the button each time round the main loop
//
// low_to_high_callback - the callback to call when the button goes from unpressed to pressed
// high_to_low_callback - the callback to call when the button goes from pressed to unpressed
// repeat_callback - the callback to call repeatedly when the button is held

void Button::update(
    void (*low_to_high_callback)(),
    void (*high_to_low_callback)(),
    void (*repeat_callback)())
    {

    // Read state of the button (invert because we're using INPUT_PULLUP)
    byte current_state = !digitalRead(pin);

    // Get current time
    unsigned long current_time = millis();

    // If the button is in a different state to the stable state
    if (current_state != stable_state) {

        // If the button is in the same state as it was last time
        if (current_state == last_read_state) {

            // If it's been in this new state for longer that the debounce time, flip the state
            // and call the callbacks
            if (current_time - last_state_change > debounce_time) {
                repeating = false;
                stable_state = current_state;
                if (current_state == HIGH) {
                    if (low_to_high_callback != NULL) {
                        low_to_high_callback();
                    }
                } else {
                    if (high_to_low_callback != NULL) {
                        high_to_low_callback();
                    }
                }
            }

        // Else the button has just changed state
        } else {
            // Set the last state and record the time
            last_read_state = current_state;
            last_state_change = current_time;
        }

    // The button is in the same state as it was last time
    } else {
        // If a repeat callback has been passed and the button is pressed
        if ((repeat_callback != NULL) && (current_state == HIGH)) {
            // If it's already repeating
            if (repeating) {
                // If enough time has passed for the next repeat call
                if (current_time - last_repeat_time > repeat_frequency) {
                    last_repeat_time = current_time;
                    repeat_callback();
                }

            // It's not repeating yet, we're waiting to start
            } else {
                // If it's been pressed for long enough
                if ((current_time - last_state_change) > repeat_delay) {
                    last_repeat_time = current_time;
                    repeat_callback();
                    repeating = true;
                }
            }
        }
    }
}













