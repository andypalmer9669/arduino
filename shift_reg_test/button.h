// Convenience class to use a button with Arduino

#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

class Button {
    public:
        Button();
        Button(byte pin_, byte debounce_time_=10);
        void init();
        void set_pin(byte pin_);
        void set_debounce_time(byte debounce_time);
        void set_repeat_delay(unsigned long repeat_delay_);
        void set_repeat_frequency(unsigned long repeat_frequency_);
        byte get_state();
        void update(void (*low_to_high_callback)()=NULL, void (*high_to_low_callback)()=NULL, void (*repeat_callback)()=NULL);

    private:
        void constructor_defaults();

        // The pin this button is connected to
        byte pin;
  
        // The time in milliseconds for the state to be held before considering it to be on
        byte debounce_time;

        // The time in milliseconds that the button needs to be held for before repeating
        unsigned long repeat_delay;

        // The time in milliseconds to between each call of the repeat callback
        unsigned long repeat_frequency;

        // Whether the button is currently repeating
        bool repeating;

        // Whether the button is pressed or not
        byte stable_state;
    
        // The state of the pin read at the last update
        byte last_read_state;
    
        // When the button last changes state (as directly read)
        unsigned long last_state_change;

        // When the repeat callback was last called
        unsigned long last_repeat_time;
};

#endif