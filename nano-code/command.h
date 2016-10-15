/**
 * command.h
 *
 *  Created on: Oct 10, 2016
 *      Author: lestarch
 */
#include <cstdint>

#ifndef COMMAND_H_
#define COMMAND_H_
    //Length in bytes to compute command checksum
	#define CHECKSUM_LENGTH 10
    //Length of command
	#define LENGTH 12
	//Error status for deserialization failure
    #define CMD_DESERIALIZE_CHECKSUM_ERROR 20

	/**
	 * Command -
	 *   A class representing a command packet sent to the fingerprint scanner
	 *   and containing the necessary values.
	 */
	class Command
	{
		private:
			uint8_t  preabmle[2] = {0x55,0xAA};
			uint16_t identifier = 0x0001;
			uint32_t param = 0;
			uint16_t cmd = NIL;
			uint16_t check = 0;
			int len = LENGTH;
		public:
			/**
			 * Command::Type -
			 *   An enumeration of the possible types the command can be set to.
			 */
			enum Type
			{
				NIL = 0x00,
				OPEN = 0x01,
				CLOSE = 0x02,
				CMOS_LED = 0x12,
				GET_ENROLL_COUNT = 0x20,
				ENROLL_START = 0x22,
				ENROLL1 = 0X23,
				ENROLL2 = 0x24,
				ENROLL3 = 0x25,
				IS_PRESS_FINGER = 0x26,
				IDENTIFY = 0x51,
				CAPTURE_FINGER = 0x60,
				ACK = 0x30,
				NAK = 0x31,
				DELETE_ALL = 0x41
			};
			/**
			 * A constructor used to build this command.
			 * \param Command::Type type - type of this command
			 */
			Command(Command::Type type);
			/**
			 * Calculates the checksum and fills it into the structure
			 * \return int - 0 on success or non-zero on error
			 */
			int checksum();
			/**
			 * Generates an array of bytes representing the serialization of this command for transmitting
			 * to the fingerprint scanner.
			 * \param uint8_t* bytes - (in/out) array of bytes for sending this command packet to scanner
			 *                          Note: user must delete this array.
			 * \return int - 0 on success or non-zero on error
			 */
			int serialize(uint8_t* bytes);
			/**
			 * Takes an array of bytes and turns it into this command object
			 * \param uint8_t* bytes - bytes to pull in as data
			 * \return int - 0 on success or non-zero as error
			 */
			int deserialize(uint8_t* bytes);
			/**
			 * Get this command's parameter
			 * \return uint32_t - param value
			 */
			uint32_t getPatameter();
			/**
			 * Set this command's parameter
			 * \param uint32_t param - parameter value to set into this command
			 */
			void setPatameter(uint32_t param);
	};
#endif
