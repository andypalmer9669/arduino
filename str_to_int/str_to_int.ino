// String inString = "";    // string to hold input

enum e_signed_mode {
    SIGNED,
    UNSIGNED
};

class blah {
    public:
        blah();
    private:
        char my_string[10];
};

blah::blah() {
    strcpy(my_string, "");
}

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }

    byte my_val = 0;
    Serial.println(my_val);
    my_val -= 1;
    Serial.println(my_val);
    my_val = 255;
    Serial.println(my_val);
    my_val += 1;
    Serial.println(my_val);

    // send an intro:
    String test_string = String(-4, DEC);    // string to hold input
    Serial.println(test_string);

    String atoitest = String("-5");
    int res = atoi(atoitest.c_str());
    Serial.println(res);

    char msg[16] = "foo";
    strcpy(msg, "hello there");
    Serial.println(msg);
    strcpy(msg, "foo");
    Serial.println(msg);
    msg[1] = 'a';
    Serial.println(msg);
    msg[3] = 'x';
    msg[10] = '\0';
    Serial.println(msg);
    strcpy(msg, "");
    Serial.println(msg);
    strcpy(msg, "bar");
    Serial.println(msg);

    char name1[] = "hello";
    char name2[] = "world";

    char * names[] = {name1, name2};
    Serial.println(names[0]);
    Serial.println(names[1]);

    e_signed_mode signed_mode = UNSIGNED;

    switch (signed_mode) {
    case SIGNED:
        Serial.println("Signed");
        break;
    case UNSIGNED:
        Serial.println("Unsigned");
        break;
    }

    foo(2);

    int test[16] = {1,2,3};
    // test = {4,5,6};

    blah my_blah();

    int maxnum = 255;

    byte_to_binary(0, msg);
    Serial.println(msg);
    byte_to_binary(64, msg);
    Serial.println(msg);
    byte_to_binary(maxnum, msg);
    Serial.println(msg);

    sprintf(msg, "%8d", 123);
    Serial.println(msg);
    sprintf(msg, "%8d", -123);
    Serial.println(msg);
    sprintf(msg, "%8X", 15);
    Serial.println(msg);

    sprintf(msg, "%-8sH", "Foo");
    Serial.println(msg);

    draw_clock_frequency(0.1);
    draw_clock_frequency(0.9999);
    draw_clock_frequency(1.0);
    draw_clock_frequency(5.345);
    draw_clock_frequency(9.99);
    draw_clock_frequency(10.0);
    draw_clock_frequency(12.3);
    draw_clock_frequency(435.4);
    draw_clock_frequency(999.9);
    draw_clock_frequency(1000.0);
    draw_clock_frequency(1001.0);
    draw_clock_frequency(12000.0);
    draw_clock_frequency(54200.0);
    draw_clock_frequency(99999.0);
    draw_clock_frequency(100000.0);
    draw_clock_frequency(123000.0);
    draw_clock_frequency(784551.0);
    draw_clock_frequency(999999.0);
    draw_clock_frequency(1000000.0);
    draw_clock_frequency(1100000.0);



}

void loop() {
// Read serial input:
// while (Serial.available() > 0) {
//   int inChar = Serial.read();
//   if (isDigit(inChar)) {
//     // convert the incoming byte to a char and add it to the string:
//     inString += (char)inChar;
//   }
//   // if you get a newline, print the string, then the string's value:
//   if (inChar == '\n') {
//     Serial.print("Value:");
//     Serial.println(inString.toInt());
//     Serial.print("String: ");
//     Serial.println(inString);
//     // clear the string for new input:
//     inString = "";
//   }
// }
}

void foo(e_signed_mode blah) {
    if (blah == UNSIGNED) {
        int a = 3;
    }
}

void byte_to_binary(byte value, char buffer[]) {
    for (int bit_index = 7; bit_index >=0; bit_index--) {
        if ((value >> bit_index) & 1) {
            buffer[7 - bit_index] = '1';
        } else {
            buffer[7 - bit_index] = '0';
        }
    buffer[8] = '\0';
    }
}

void draw_clock_frequency(float frequency) {
    char freq_buf[16];
    char print_buf[16];

    // 0.0 - 9.9
    if (frequency < 10.0) {
        Serial.print("In: ");
        Serial.print(frequency);
        Serial.print(" Out: ");
        int mult_frequency = frequency * 10.0;
        sprintf(freq_buf, "%02d", mult_frequency);
        print_buf[0] = freq_buf[0];
        print_buf[1] = '.';
        print_buf[2] = freq_buf[1];
        print_buf[3] = '\0';
        Serial.println(print_buf);

    // 10 - 999
    } else if (frequency < 1000.0) {
        Serial.print("In: ");
        Serial.print(frequency);
        Serial.print(" Out: ");
        sprintf(freq_buf, "%3ld", (long) frequency);
        strncpy(print_buf, freq_buf, 3);
        print_buf[3] = '\0';
        Serial.println(print_buf);

    // 1K - 99K
    } else if (frequency < 99999.999) {
        Serial.print("In: ");
        Serial.print(frequency);
        Serial.print(" Out: ");
        sprintf(freq_buf, "%5ld", (long) frequency);
        strncpy(print_buf, freq_buf, 3);
        print_buf[2] = 'K';
        print_buf[3] = '\0';
        Serial.println(print_buf);


    // .1M - .9M
    } else if (frequency < 999999.999) {
        Serial.print("In: ");
        Serial.print(frequency);
        Serial.print(" Out: ");
        sprintf(freq_buf, "%6ld", (long) frequency);
        print_buf[0] = '.';
        print_buf[1] = freq_buf[0];
        print_buf[2] = 'M';
        print_buf[3] = '\0';
        Serial.println(print_buf);

    // 1M - ??
    } else {
        Serial.print("In: ");
        Serial.print(frequency);
        Serial.print(" Out: ");
        sprintf(freq_buf, "%8ld", (long) frequency);
        strncpy(print_buf, freq_buf, 3);
        print_buf[2] = 'M';
        print_buf[3] = '\0';
        Serial.println(print_buf);
    }
}