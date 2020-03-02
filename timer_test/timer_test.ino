#include "Arduino.h"
#include "TimerOne.h"
 
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Timer1.initialize(1000000);         // initialize timer1, and set a 
    Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
}
 
void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1500);
    Timer1.setPeriod(900000);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1500);
    Timer1.setPeriod(1100000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
    Timer1.setPeriod(700000);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1500);
    Timer1.setPeriod(900000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
    Timer1.setPeriod(700000);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1500);
    Timer1.setPeriod(800000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
    Timer1.setPeriod(1100000);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1500);
    Timer1.setPeriod(1000000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);



    // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);
    // Timer1.setPwmDuty(9, 700);
    // digitalWrite(LED_BUILTIN, HIGH); 
    // delay(1000);
    // Timer1.setPwmDuty(9, 1000);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(1000);
    // Timer1.setPwmDuty(9, 700);
    // digitalWrite(LED_BUILTIN, HIGH); 
    // delay(1000);
    // Timer1.setPwmDuty(9, 512);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(1000);
    // Timer1.setPwmDuty(9, 200);
    // digitalWrite(LED_BUILTIN, HIGH); 
    // delay(1000);
    // Timer1.setPwmDuty(9, 20);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(1000);
    // Timer1.setPwmDuty(9, 200);
    // digitalWrite(LED_BUILTIN, HIGH); 
    // delay(1000);
    // Timer1.setPwmDuty(9, 512);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(1000);
}