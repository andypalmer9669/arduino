#define HUE_CHANGE_THRESHOLD 20

class Cycler {
    public:
        enum mode_t {CYCLE_HUE, PULSE_VALUE, PULSE_VALUE_CHANGE_HUE};
    private:
        // How muct to increment _value by in value/millisecond
        float _value_timebase_speed;

        // The current value
        float _value_timebase;

        // The value
        uint8_t _value

        // How much to increment _hue by in hue/milllisecond
        float _hue_speed;

        // Current hue
        float _hue;

        uint8_t _hue_change_threshold;

        // When the last update was
        unsigned long _last_update;

        // The current mode
        mode_t _mode;

    public:
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
                }
            }
        }

        void update_cycle_hue(uint8_t elapsed_millis) {
            float new_hue = _hue + (elapsed_millis * _hue_speed);
            if (new_hue > 256.0) {
                _hue = new_hue - 256.0;
            } else {
                _hue = new_hue;
            }
        }

        void update_pulse_value(uint8_t elapsed_millis) {
            float new_value_timebase = _value_timebase + (elapsed_millis * _value_timebase_speed)
            if (new_value_timebase > 256.0) {
                _value_timebase = new_value_timebase - 256.0;
            } else {
                _value_timebase = new_value_timebase;
            }

            _value = sin8(uint8_t(_value_timebase));
        }

        void update_pulse_value_change_hue(uint8_t elapsed_millis) {
            uint8_t _previous_value = _value;
            update_pulse_value(elapsed_millis);
            if ((_previous_value >= HUE_CHANGE_THRESHOLD) && (_value < HUE_CHANGE_THRESHOLD)) {
                _hue = uintrandom8();
            }
        }

        uint8_t get_hue() {
            return _hue;
        }

        uint8_t get_value() {
            return uint8_t(_value);
        }
};
















