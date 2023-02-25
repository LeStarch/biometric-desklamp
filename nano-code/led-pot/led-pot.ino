#include "config.h"


#define WINDOW_SIZE 100
#define EDGE_CUT 100
#define MAX_INPUT 1024
#define MAX_OUTPUT 255
#define OUT_33 85

void setup() {
    //Serial.begin(115200);
    //delay(500);
    pinMode(LED_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
}

unsigned long int reading() {
    static bool s_filled = false;
    static unsigned long int s_index = 0;
    static unsigned long int s_window[WINDOW_SIZE];

    s_window[s_index] = analogRead(A0);
    s_index = (s_index + 1) % WINDOW_SIZE;
    s_filled = (s_index == 0);
    // Calculate average window
    unsigned long int average = 0;
    for (unsigned long int i = 0; i < (s_filled ? WINDOW_SIZE : s_index); i++) {
        average += s_window[i];
    }
    average = average / ((s_filled) ? WINDOW_SIZE : s_index);
    return average;
}

void loop() {
    unsigned long int val = reading();
    // Map the two edge values to off or max 
    if (val < EDGE_CUT) {
        val = 0;
    } else if (val > (MAX_INPUT - EDGE_CUT)) {
        val = MAX_OUTPUT;
    }
    // Otherwise run from 33% - 66%
    else {
        val = (val * OUT_33) / MAX_INPUT + OUT_33;
    }
    //Serial.print("Value:");
    //Serial.print(val);
    //Serial.print("\n");
    // Write output   
    analogWrite(LED_PIN, val);
    analogWrite(FAN_PIN, val);
}
