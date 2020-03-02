#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

void setup() {
    lcd.init();
    lcd.backlight();
    lcd.print("Hello World!");
}

void loop() {
    lcd.cursor();
    delay(500);
    lcd.noCursor();
    delay(500);
}