#include <stdint.h>

//Error pin to flash on error
int ERROR_PIN = 13;
/**
 * Enumeration of commands
 * Value set to control code of command
 * See datasheet for fingerprint scanner
 **/
typedef enum 
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
  NO_ACK = 0x31
  
} Command;

/**
 * Command structure
 */
typedef struct
{
  uint8_t start1;
  uint8_t start2;
  uint16_t did;
  uint32_t param;
  uint16_t cmd;
  uint16_t check;
  //Notice: This lenght doesn't include itself.
  //Thus if treated as byte[], this is ignored.
  long len;
} CommandPacket;


void setup();
void loop();
void error(int error);
CommandPacket* getCommand(Command cmd);
void setParameter(CommandPacket* packet,uint16_t value);
void fillChecksum(uint8_t* bytes,long len);
CommandPacket* sendCommand(CommandPacket* packet);
void sendBytes(uint8_t* bytes,long len);
CommandPacket* readCommand();
void readBytes(uint8_t* bytes,long len);

/**
 * Setups up the baudrate to be 9600 and
 * init scanner
 */
void setup()
{
  pinMode(ERROR_PIN, OUTPUT);
  Serial.begin(9600);
  //Wait for the serial pins to connect (May only be needed for Leonardo board)
  while (!Serial) {}
  CommandPacket* init = getCommand(OPEN);
  CommandPacket* resp = sendCommand(init);
  free(init);
  free(resp);
}
/**
 * Flashes LED
 */
void loop()
{
  CommandPacket* led = getCommand(CMOS_LED);
  CommandPacket* res;
  while (true)
  {
    setParameter(led,1);
    res = sendCommand(led);
    //if (res->cmd == ACK)
    digitalWrite(ERROR_PIN, LOW);
    //else
    //  digitalWrite(ERROR_PIN, LOW);
    free(res);
    delay(5000);
    setParameter(led,0);
    res = sendCommand(led);
    //if (res->cmd == ACK)
      digitalWrite(ERROR_PIN, HIGH);
    //else
    //  digitalWrite(ERROR_PIN, HIGH);
    free(res);
    delay(5000);      
  }
}
/**
 * Flash arduino LED on error
 * error - number of times to flash
 */
void error(int error)
{
  Serial.end();
  int i = 0;
  while(true)
  {
    //Flash "error" number of times.
    for (i = 0; i < error; i++)
    {
      digitalWrite(ERROR_PIN, HIGH);
      delay(200);
      digitalWrite(ERROR_PIN, LOW);
      delay(200);
    }
    delay(1800);
  }
}

/**
 * Get a CommandPacket that represents given command.
 * cmd - command enumeration value of desired command
 * returns - command packet representing command
 */
CommandPacket* getCommand(Command cmd)
{
  CommandPacket* packet = (CommandPacket*)malloc(sizeof(CommandPacket));
  packet->start1 = (uint8_t)0x55;
  packet->start2 = (uint8_t)0xAA;
  packet->did = (uint16_t)0x0001;
  packet->len = 12;
  setParameter(packet,(uint16_t) 0);
  //Sanity checking
  if (sizeof(CommandPacket) != packet->len + 4)
    error(1);
  packet->cmd=(uint16_t)cmd;
  return packet;
}
/**
 * Sets the aparameter of command packet
 * packet - packet to set value for
 * value - value to set parameter to
 */
void setParameter(CommandPacket* packet,uint16_t value) 
{
  packet->param = value;
}
/**
 * Fils the last 2 bytes of the byte array with a straight additiom checksum.
 * unsigned char* bytes - byte artray to take checksum of.
 * int len - number of bytes
 */
void fillChecksum(uint8_t* bytes,long len) {
  //Adjust length to not include checksum bytes
  len = len - 2;
  uint16_t* check = (uint16_t*)(bytes + len);
  *check = 0;
  //Checksum is commutative, so it can be done in reverse
  while (--len >= 0)
  {
    *check += *(bytes+len);
  }
}
/**
 * Send a command packet recieves responce
 * packet - command packet to send
 */
CommandPacket* sendCommand(CommandPacket* packet)
{
  sendBytes((uint8_t*)packet,packet->len);
  return readCommand();
}
/**
 * Sends bytes.  All is little endian so no conversion necessary.
 * bytes - pointer to bytes to write
 * len - length of bytes
 */
void sendBytes(uint8_t* bytes,long len) 
{
  fillChecksum(bytes,len);

  if (Serial.write(bytes,len) != len)
    error(10);
}
/**
 * Read a command packet
 * packet - command packet to send
 */
CommandPacket* readCommand()
{
  //Get empty command so memory is zeroed, and len is set
  CommandPacket* packet = getCommand(NIL);
  readBytes((uint8_t*)packet,packet->len);
}
/**
 * Reads bytes.  All is little endian so no conversion necessary.
 * bytes - pointer to bytes to read into
 * len - length of bytes
 */
void readBytes(uint8_t* bytes,long len)
{
  //Read bytes waiting if data is not found
  long i = 0;
  for (i = 0; i < len; i++)
  {
    int b = Serial.read();
    if (b == -1)
      delay(10);
    bytes[i] = (uint8_t) b;
  }
}

