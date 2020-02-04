/*
 * scanner.h
 *
 *  Created on: Oct 11, 2016
 *      Author: starchmd
 */
#include <cstdint>
#include <Arduino.h>

#include "command.h"

#ifndef SCANNER_H_
#define SCANNER_H_
	//Time to delay between flashes
	#define FLASH_DELAY 50
	//Number of flashes for the enroll peiod
    #define ENROLL_FLASH_COUNT 3
	//Error status on serial write failure
    #define SERIAL_WRITE_FAILURE 1
	/**
	 * Scanner -
	 *   A class representing a fingerprint scanner's UART interface.
	 */
	class Scanner
	{
		private:
			HardwareSerial* serial;
		public:
			/**
			 * Enumeration of the states for the CMOS LED
			 */
			enum LedState
			{
				ON = 1,
				OFF = 0
			};
			/**
			 * A constructor used to build this Scanner.
			 * \param HardwareSerial serial - serial object
			 */
			Scanner(HardwareSerial& serial);
			/**
			 * Turn on or off the LED
			 * \param state - state (ON/OFF) to set LED to
			 * \return 0 on success, anything else on error
			 */
			int led(LedState state);
			/**
			 * Flush the serial port of extra data
			 * \return 0 on success, anything else on error
			 */
			int flush();
			/**
			 * Flash the CMOS LED
			 * \param int count - number of times to flash per group
			 * \param int pause - time to delay between flash groups
			 * \param int repeat - number of times to repeat flash group
			 * \return 0 on success, anything else on error
			 */
			int flash(int count, int pause, int repeat);
			/**
			 * Capture a fingerprint
			 * \param bool detailed - should a longer capture (with finger removal) be used
			 * \return 0 on success, anything else on error
			 */
			int capture(bool detailed);
			/**
			 * Capture and identify if know fingerprint
			 * \param bool known - (output) filled with true if finger is known, false otherwise
			 * \return 0 on success, anything else on error
			 */
			int indentify(bool &known);
			/**
			 * Enroll a fingerprint
			 * \param Command::Type type - used for recursion, Enroll subcommand (start, 1, 2, or 3)
			 * \param uint32_t param - fingerprint id to supply to enroll_start (or zero)
			 * \return 0 on success, anything else on error
			 */
			int enroll(Command::Type type, uint32_t param);
			/**
			 * Send the given command, and receive the response
			 * \param Command command - (input) command to send
			 * \param Command response - (output) response to command
			 * \return 0 on success, anything else on error
			 */
			int send(Command& command,Command& response);
			/**
			 * Helper to send, for receiving a response
			 * \param Command response - (output) response received
			 * \return 0 on success, anything else on error
			 */
			int recv(Command& response);
	};
#endif
