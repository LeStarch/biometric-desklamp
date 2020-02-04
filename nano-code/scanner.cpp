/*
 * scanner.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: starchmd
 */
#include <cstdint>
#include <cstddef>
#include <Arduino.h>

#include "command.h"
#include "scanner.h"
#include "config.h"
/**
 * A constructor used to build this Scanner.
 * \param HardwareSerial serial - serial object
 */
Scanner::Scanner(HardwareSerial& serial)
{
	//Since the HWSerial is global, then keeping an internal pointer
	//should be ok
	this->serial = &serial;
	this->serial->begin(9600);
}
/**
 * Turn on or off the LED
 * \param state - state (ON/OFF) to set LED to
 * \return 0 on success, anything else on error
 */
int Scanner::led(LedState state)
{
    Command led(Command::CMOS_LED);
    Command ret(Command::NIL);
    led.setPatameter(state);
    return this->send(led,ret);
}
/**
 * Flush the serial port of extra data
 * \return 0 on success, anything else on error
 */
int Scanner::flush()
{
	int status = 0;
	int item = 0xFFFF;
	delay(10);
    #ifdef DEBUG
	    if (this->serial->available() > 0)
	    {
	        Serial.print("Flushing: ");
	    }
    #endif
	while(this->serial->available() > 0)
	{
		item = this->serial->read();
        #ifdef DEBUG
		    Serial.print(item);
		    Serial.print(",");
        #endif
	}
    #ifdef DEBUG
	    if (item != 0xFFFF)
	    {
            Serial.println();
	    }
    #endif
    return status;
}
/**
 * Flash the CMOS LED
 * \param int count - number of times to flash per group
 * \param int pause - time to delay between flash groups
 * \param int repeat - number of times to repeat flash group
 * \return 0 on success, anything else on error
 */
int Scanner::flash(int count, int pause, int repeat)
{
	int status = 0;
    //Repeat a lot
	while (repeat > 0)
	{
        for (int i = 0; i < count; i++)
        {
        	if ((status = this->led(Scanner::ON)) != 0)
        	{
        		return status;
        	}
        	delay(FLASH_DELAY);
        	if ((status = this->led(Scanner::OFF)) != 0)
        	{
                return status;
        	}
        	delay(FLASH_DELAY);
        }
        delay(pause);
        repeat--;
	}
	return 0;
}
/**
 * Capture a fingerprint
 * \param bool detailed - should a longer capture (with finger removal) be used
 * \return 0 on success, anything else on error
 */
int Scanner::capture(bool detailed)
{
	int status = 0;
	Command ret(Command::NIL);
	Command finger(Command::IS_PRESS_FINGER);
	Command cap(Command::CAPTURE_FINGER);
    cap.setPatameter(0);
	#ifdef DEBUG
		Serial.println("-- Capturing --");
	#endif
	if ((status = this->led(Scanner::ON)) != 0)
	{
		return status;
	}
	//Prime finger checking
	if ((status = this->send(finger,ret)) != 0)
	{
		return status;
	}
	//Detailed prints require removing the finger
	if (detailed)
	{
		//Wait for figure to be removed from scanner
		#ifdef DEBUG
			Serial.println("-- Waiting for Finger Removal --");
		#endif
		while (ret.getPatameter() == 0)
		{
			if ((status = this->send(finger,ret)) != 0)
			{
				return status;
			}
		}
		cap.setPatameter(1);
	}
    //Wait for finger to be placed back
	#ifdef DEBUG
		Serial.println("-- Waiting for Finger --");
	#endif
    while (ret.getPatameter() != 0)
    {
    	if ((status = this->send(finger,ret)) != 0)
        {
        	return status;
        }
    }
    //Run the capture command
    if ((status = this->send(cap,ret)) != 0)
    {
    	return status;
    }
    return 0;
}
/**
 * Capture and identify if know fingerprint
 * \param bool known - (output) filled with true if finger is known, false otherwise
 * \return 0 on success, anything else on error
 */
int Scanner::indentify(bool &known)
{
	int status;
	Command ident(Command::IDENTIFY);
	Command ret(Command::IDENTIFY);
	known = false;
	if ((status = this->capture(false)) != 0)
	{
	    return status;
	}
    //Check for status
	if ((status =this->send(ident,ret)) != 0)
	{
	    return status;
	}
	//If the identity is not 199 (unknown) then return "known"
	if (ret.getPatameter() < 199)
	{
	    known = true;
	}
	return 0;
}
/**
 * Enroll a fingerprint
 * \param Command::Type type - used for recursion, Enroll subcommand (start, 1, 2, or 3)
 * \param uint32_t param - fingerprint id to supply to enroll_start (or zero)
 * \return 0 on success, anything else on error
 */
int Scanner::enroll(Command::Type type, uint32_t param)
{
	Command cmd(type);
	Command ret(Command::NIL);
    #ifdef DEBUG
        Serial.print("-- Enrolling: ");
        Serial.print(type-Command::ENROLL_START);
        Serial.println(" --");
    #endif
	//Capture fingerprints for enroll
	if (type >= Command::ENROLL1 && type <= Command::ENROLL3)
    {
        this->capture(true);
        this->led(Scanner::OFF);
        delay(TOGGLE_DELAY);
        this->led(ON);
    }
    //Enroll commands in order: START, 1,2,3
    switch(type)
    {
	    case Command::ENROLL_START:
	        cmd.setPatameter(param);
	    case Command::ENROLL1:
        case Command::ENROLL2:
        case Command::ENROLL3:
            this->send(cmd,ret);
	        this->enroll((Command::Type)(type+1),0);
	    default:
	        break;
    }
    return 0;
}
/**
 * Send the given command, and receive the response
 * \param Command command - (input) command to send
 * \param Command response - (output) response to command
 * \return 0 on success, anything else on error
 */
int Scanner::send(Command& command, Command& response)
{
	int status = 0;
	uint8_t bytes[LENGTH];
	//Force clearing of buffer
    this->flush();
    //Fill in the checksum
	if ((status = command.checksum()) != 0)
    {
        return status;
    }
    //Serialize the command
    if ((status = command.serialize(bytes)) != 0)
    {
       return status;
    }
    if (this->serial->write((const uint8_t *)bytes, (size_t)LENGTH) != LENGTH)
    {
		#ifdef DEBUG
    	    Serial.println("Failed to write all bytes");
		#endif
    	return SERIAL_WRITE_FAILURE;
    }
    if ((status = this->recv(response)) != 0)
    {
    	return status;
    }
    return 0;
}
/**
 * Helper to send, for receiving a response
 * \param Command response - (output) response received
 * \return 0 on success, anything else on error
 */
int Scanner::recv(Command& command)
{
    int status = 0;
	uint8_t bytes[LENGTH];
	this->serial->readBytes(bytes, LENGTH);
    #ifdef DEBUG
        Serial.print("Rec'd bytes:");
        for (int i = 0; i < LENGTH; i++)
        {
        	Serial.print((int)bytes[i]);
        	Serial.print(",");
        }
        Serial.println();
	#endif
    command.deserialize(bytes);
	return status;
}

