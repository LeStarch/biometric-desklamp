/*
 * main.c
 *
 *  Created on: Oct 9, 2016
 *      Author: starchmd
 */
#include <Arduino.h>

#include "scanner.h"
#include "led.h"
#include "config.h"

//Global scanner pointer
Scanner* scanner;
//Global led pointer
Led* led;
/**
 * Check if we need to enroll and then do so
 * \return 0 on success, anything else on error
 */
int check_and_enroll()
{
    int status = 0;
    uint32_t count;
    uint32_t time;
    Command roll(Command::GET_ENROLL_COUNT);
    Command finger(Command::IS_PRESS_FINGER);
    Command ret(Command::NIL);
    #ifdef DEBUG
        Serial.println("-- Checking Enrollment --");
    #endif
    //Check if there are enrolled fingers
    if ((status = scanner->send(roll,ret)) != 0)
    {
        return status;
    }
    count = ret.getPatameter();
	//Enroll count != 1 ot
    if ((status = scanner->flash(ENROLL_FLASH_COUNT, 0, 1)) != 0)
    {
        return status;
    }
    scanner->led(Scanner::ON);
    time = millis()+ENROLL_PERIOD_TIME;
    while (millis() < time)
    {
    	if ((status = scanner->send(finger,ret)) != 0)
    	{
    		return status;
    	}
    	if (count == 0 || ret.getPatameter() == 0)
    	{
			#ifdef DEBUG
				Serial.print("Need to enroll: ");
				Serial.print(count);
				Serial.print(" ");
				Serial.println(ret.getPatameter());
			#endif
			if ((status = scanner->enroll(Command::ENROLL_START, count)) != 0)
			{
				return status;
			}
			break;
    	}
    }
    status = scanner->flash(ENROLL_FLASH_COUNT, 0, 1);
    return status;
}
/**
 * Handle an error by flashing the status on the led
 * \param int status - status to flash
 */
void error(int status)
{
	#ifdef DEBUG
		Serial.print("Error detected: ");
		Serial.println(status);
	#endif
	scanner->flash(status, 1000, 10);
}
/**
 * Setup to run the loop
 */
void setup()
{
    Command open(Command::OPEN);
    Command ret(Command::NIL);
    int status = 0;
	#ifdef DEBUG
    	Serial.begin(9600);
    	delay(1000);
    	Serial.println("Starting serial debugger");
    #endif
	scanner = new Scanner(Serial1);
	scanner->flush();
	scanner->send(open,ret);
	scanner->led(Scanner::OFF);
	delay(START_UP_DELAY);
    scanner->recv(ret);
	led = new Led(LED_PIN,FAN_PIN);
    if ((status = check_and_enroll()) != 0)
    {
        error(status);
    }
}
/**
 * Loop forever
 */
void loop()
{
	int status = 0;
	bool known = false;
    if((status = scanner->indentify(known)) != 0)
    {
    	error(status);
    }
    else if (known)
    {
    	led->toggle();
    	scanner->led(Scanner::OFF);
    	delay(TOGGLE_DELAY);
    	scanner->led(Scanner::ON);
    }
}
