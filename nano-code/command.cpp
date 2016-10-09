#include <cstdint>
#include <command.h>
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
int checksum()
{
    int status = 0;
    uint16_t checksum = 0;
    uint8_t* bytes = NULL;
    if ((status = this->serialize(&bytes)) != 0)
    {
         return status;
    }
    for (int i = 0; i < CHECKSUM_LENGTH; i++)
    {
        checksum += bytes[i];
    }
    delete bytes;
    this->check = checksum;
}
/**
 * Generates an array of bytes representing the serilization of this command for transmitting
 * to the fingerprint scanner.
 * \param uint8_t** bytes - (out) array of bytes for sending this command packet to scanner
 *                          Note: user must delete this array.
 * \return int - 0 on success or non-zero on error
 */
int serialize(uint8_t** bytes)
{
    try
    {
        bytes = 
    }
}
/**
 * Set this command's parameter
 * \param uint32_t param - parameter value to set into this command
 */
void setPatameter(uint32_t param)
{
    this->param = param; 
}
