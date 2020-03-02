#include <Arduino.h>
#include "button.h"

#define BUTTON_PIN 12
#define SERIAL_READ_DATA_PIN 8
#define SERIAL_READ_CLOCK_PIN 7
#define SERIAL_READ_SHIFTLOAD_PIN 6
#define SERIAL_WRITE_DATA_PIN 9
#define SERIAL_WRITE_CLOCK_PIN 10
#define SERIAL_WRITE_LATCH_PIN 11

Button test_button(BUTTON_PIN);
char print_buffer[] = "                ";

void setup() {
    // Called once at Arduino startup.

    pinMode(SERIAL_READ_DATA_PIN, INPUT);
    pinMode(SERIAL_READ_CLOCK_PIN, OUTPUT);
    pinMode(SERIAL_READ_SHIFTLOAD_PIN, OUTPUT);
    pinMode(SERIAL_WRITE_DATA_PIN, OUTPUT);
    pinMode(SERIAL_WRITE_CLOCK_PIN, OUTPUT);
    pinMode(SERIAL_WRITE_LATCH_PIN, OUTPUT);

    digitalWrite(SERIAL_READ_SHIFTLOAD_PIN, HIGH);
    digitalWrite(SERIAL_READ_CLOCK_PIN, LOW);
    digitalWrite(SERIAL_WRITE_CLOCK_PIN, LOW);
    digitalWrite(SERIAL_WRITE_LATCH_PIN, LOW);

    test_button.init();

    Serial.begin(9600);
    // wait for serial port to connect. Needed for native USB port only
    while (!Serial) {
    ; 
    }

}

void loop() {
    // Main loop function for arduino.
    test_button.update(&shift_test, NULL, NULL);
}

void shift_test() {

    // Get Value
    byte incoming = shift_in(SERIAL_READ_DATA_PIN, SERIAL_READ_CLOCK_PIN, SERIAL_READ_SHIFTLOAD_PIN);
    byte_to_binary_string(incoming, print_buffer);
    Serial.println(print_buffer);

    // Send value back out
    digitalWrite(SERIAL_WRITE_CLOCK_PIN, LOW);
    shiftOut(SERIAL_WRITE_DATA_PIN, SERIAL_WRITE_CLOCK_PIN, LSBFIRST, incoming);
    digitalWrite(SERIAL_WRITE_CLOCK_PIN, LOW);
    digitalWrite(SERIAL_WRITE_LATCH_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(SERIAL_WRITE_LATCH_PIN, LOW);









    // Get Value
    // digitalWrite(SERIAL_READ_SHIFTLOAD_PIN, LOW);
    // delayMicroseconds(5);
    // digitalWrite(SERIAL_READ_SHIFTLOAD_PIN, HIGH);

    // digitalWrite(SERIAL_READ_CLOCK_PIN, HIGH);
    // delayMicroseconds(5);

    // byte incoming = shiftIn(SERIAL_READ_DATA_PIN, SERIAL_READ_CLOCK_PIN, LSBFIRST);
    // Serial.println(incoming);

    // byte_to_binary_string(incoming, print_buffer);
    // Serial.println(print_buffer);

    // Send value back out
    // digitalWrite(SERIAL_WRITE_LATCH_PIN, LOW);
    // shiftOut(SERIAL_WRITE_DATA_PIN, SERIAL_WRITE_CLOCK_PIN, LSBFIRST, incoming);
    // digitalWrite(SERIAL_WRITE_LATCH_PIN, HIGH);
}


byte shift_in(byte data_pin, byte clock_pin, byte shiftload_pin) {
    byte current_bit = 0;
    byte result = 0;

    digitalWrite(clock_pin, LOW);

    digitalWrite(shiftload_pin, LOW);
    delayMicroseconds(5);
    digitalWrite(shiftload_pin, HIGH);

    for(int bit_index = 0; bit_index < 8; ++bit_index) {
        current_bit = digitalRead(data_pin);
        result |= current_bit << bit_index;
        digitalWrite(clock_pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(clock_pin, LOW);
    }

    return result;
}


void byte_to_binary_string(byte value, char buffer[]) {
    for (int bit_index = 7; bit_index >=0; bit_index--) {
        if ((value >> bit_index) & 1) {
            buffer[7 - bit_index] = '1';
        } else {
            buffer[7 - bit_index] = '0';
        }
    buffer[8] = '\0';
    }
}