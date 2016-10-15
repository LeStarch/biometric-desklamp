/**
 * led.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: lestarch
 */
#include <cstdint>
#include <Arduino.h>

#include "led.h"
#include "config.h"
/**
 * Construct this led-module and fan around the supplied pins
 */
Led::Led(int led,int fan)
{
     this->led = led;
     this->fan = fan;
     this->rate = 0;
     pinMode(this->led,OUTPUT);
     pinMode(this->fan,OUTPUT);
}
/**
 * Toggles the led and fan to the next in a certain number of steps
 */
void Led::toggle()
{
	uint16_t rled = 0;
	uint16_t rfan = 0;
	this->rate = (this->rate <= 0)?MAX_RATE:this->rate - MAX_RATE/TOGGLE_STEPS;
    rled = (this->rate * 256)/100;
    rfan = (min((this->rate*125)/100,100u)*256)/100;
    #ifdef DEBUG
        Serial.print("---- Toggle to: ");
        Serial.print(rled);
        Serial.println(" ----");
    #endif
   analogWrite(this->led,rled);
   analogWrite(this->fan,rfan);
}
