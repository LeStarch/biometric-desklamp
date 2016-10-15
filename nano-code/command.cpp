/**
 * command.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: lestarch
 */
#include <cstdint>
#include <cstring>

#include "command.h"
#include "config.h"
/**
* A constructor used to build this command.
* \param Command::Type type - type of this command 
*/
Command::Command(Command::Type type)
{
    this->cmd = (uint16_t) type;
}
/**
 * Calculates the checksum and fills it into the structure
 * \return int - 0 on success or non-zero on error
 */
int Command::checksum()
{
    int status = 0;
    uint16_t checksum = 0;
    uint8_t bytes[LENGTH];
    if ((status = this->serialize(bytes)) != 0)
    {
         return status;
    }
    for (int i = 0; i < CHECKSUM_LENGTH; i++)
    {
        checksum += bytes[i];
    }
    this->check = checksum;
    return 0;
}
/**
 * Generates an array of bytes representing the serialization of this command for transmitting
 * to the fingerprint scanner.
 * \param uint8_t* bytes - (in/out) array of bytes for sending this command packet to scanner
 *                          Note: user must delete this array.
 * \return int - 0 on success or non-zero on error
 */
int Command::serialize(uint8_t* bytes)
{
    memcpy((void*)bytes, (const void*)this, LENGTH);
    return 0;
}
/**
 * Takes an array of bytes and turns it into this command object
 * \param uint8_t* bytes - bytes to pull in as data
 * \return int - 0 on success or non-zero as error
 */
int Command::deserialize(uint8_t* bytes)
{
	int check;
    this->preabmle[0] = bytes[0];
    this->preabmle[1] = bytes[1];
    this->identifier = *((uint16_t*)(bytes+2));
    this->param = *((uint32_t*)(bytes+4));
    this->cmd = *((uint16_t*)(bytes+8));
    check = bytes[10];
    this->checksum();
    if (this->check != check)
    {
        return CMD_DESERIALIZE_CHECKSUM_ERROR;
    }
    else if (this->cmd == Command::NAK)
    {
    	return this->param;
    }
    return 0;
}
/**
 * Get this command's parameter
 * \return uint32_t - param value
 */
uint32_t Command::getPatameter()
{
    return this->param;
}
/**
 * Set this command's parameter
 * \param uint32_t param - parameter value to set into this command
 */
void Command::setPatameter(uint32_t param)
{
    this->param = param; 
}
