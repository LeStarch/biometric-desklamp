#include <stdint.h>

//Error pin to flash on error
int ERROR_PIN = 13;
int PWM_PIN = 3;
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
  NAK = 0x31,
  DELETE_ALL = 0x41
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

//Builtin functions
void setup();
void loop();
//Table of Contents functions
void start();
void enroll(Command cmd, uint32_t param);
//Utility functions
void toggle();
void flash(uint32_t num,int time);
//Error leaf function
void error(uint32_t error);
//Send packet functions
CommandPacket* getCommand(Command cmd);
void setParameter(CommandPacket* packet,uint16_t value);
void fillChecksum(uint8_t* bytes,long len);
uint32_t sendCommand(CommandPacket* packet);
//Read packet functions
CommandPacket* readCommand();
//Bytes read send
void sendBytes(uint8_t* bytes,long len);
void recvBytes(uint8_t* bytes,long len);

/**
 * Setups up the baudrate to be 9600 and
 * init scanner
 */
void setup()
{
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  Serial.begin(9600);
  //Wait for the serial pins to connect (May only be needed for Leonardo board)
  while (!Serial) {}
  //Turn on light
  CommandPacket* led = getCommand(CMOS_LED);
  setParameter(led,1);
  sendCommand(led);
  delay(500);
  setParameter(led,0);
  sendCommand(led);
  free(led);
  //Start: enroll if needed
  //led = getCommand(DELETE_ALL);
  //sendCommand(led);
  //free(led);
  start();
}
/**
 * Flash the CMOS LED
 * num - number of times to flash
 */
void flash(uint32_t num,int time)
{
  CommandPacket* led = getCommand(CMOS_LED);
  while (num-- > 0) {
    setParameter(led,1);
    sendCommand(led);
    delay(time);
    setParameter(led,0);
    sendCommand(led);
    delay(time);
  }
  free(led);
}
/**
 * Start by enrolling, if none enrolled
 */
void start() {
  CommandPacket* cmd = getCommand(OPEN);
  sendCommand(cmd);
  free(cmd);
  //Get number enrolled...if zero, enroll
  cmd = getCommand(GET_ENROLL_COUNT);
  uint32_t cnt = sendCommand(cmd);
  free(cmd);
  //Is there a finger
  cmd = getCommand(IS_PRESS_FINGER);
  uint32_t fin = sendCommand(cmd);
  free(cmd);
  //If no figers, or a finger is actively on scanner
  //Learn a new one
  if (cnt == 0 || fin  == 0)
  {
    //Signal enroll
    flash(3,300);
    enroll(ENROLL_START,cnt);
  }
}
/**
 * Enroll.  Recursively runs the command 4 times.
 * cmd - command ENROLL_START, ENROLL!, ENROLL2, ENROLL3
 * param - id to enroll (used only for start)
 */
void enroll(Command cmd, uint32_t param)
{
  //LED on for capture
  CommandPacket* led = getCommand(CMOS_LED);
  setParameter(led,1);
  sendCommand(led);
  //For enroll commands wait until finger pressed 
  CommandPacket* finger = getCommand(IS_PRESS_FINGER);
  while (cmd >= ENROLL1 && cmd <= ENROLL3 && sendCommand(finger) != 0) {}
  free(finger);
  //Enroll commands in order: START, 1,2,3
  CommandPacket* packet = getCommand(cmd);
  switch(cmd) {
    case ENROLL_START:
      setParameter(packet,param);
    case ENROLL1:
    case ENROLL2:
    case ENROLL3:
      param = sendCommand(packet);
      setParameter(led,0);
      sendCommand(led);
      delay(500);
      enroll((Command)(cmd+1),0);
    case ENROLL3+1:
      break;
  }
  free(packet);
  free(led);
}
/**
 * Toggle the light.
 */
void toggle() {
  static uint8_t out = 0;
  out = (out == 0) ? 99 : out - 33; 
  analogWrite(PWM_PIN,(((uint16_t)out)*256)/100);
}

/**
 * Loops waiting for finger print.
 */
void loop()
{
  CommandPacket* led = getCommand(CMOS_LED);
  setParameter(led,1);
  sendCommand(led);
  CommandPacket* finger = getCommand(IS_PRESS_FINGER);
  while (sendCommand(finger) != 0) {}
  free(finger);
  CommandPacket* id = getCommand(IDENTIFY);
  if(sendCommand(id) < 199) {
    toggle();
    setParameter(led,0);
    sendCommand(led);
    delay(500);
  }
  free(id);
  free(led);

}
/**
 * Flash arduino LED on error
 * error - number of times to flash
 */
void error(uint32_t error)
{
  Serial.end();
  int i = 0;
  while(true)
  {
    flash(error,500);
    delay(1500);
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
 * return - parameter from command
 */
uint32_t sendCommand(CommandPacket* packet)
{
  sendBytes((uint8_t*)packet,packet->len);
  packet = readCommand();
  //If NAK error, otherwise return param
  uint32_t param = packet->param;
  uint16_t cmd = packet->cmd;
  free(packet);
  return param;
}
/**
 * Sends bytes.  All is little endian so no conversion necessary.
 * bytes - pointer to bytes to write
 * len - length of bytes
 */
void sendBytes(uint8_t* bytes,long len) 
{
  fillChecksum(bytes,len);
  int i = 0;
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
  recvBytes((uint8_t*)packet,packet->len);
  return packet;
}
/**
 * Reads bytes.  All is little endian so no conversion necessary.
 * bytes - pointer to bytes to read into
 * len - length of bytes
 */
void recvBytes(uint8_t* bytes,long len)
{
  //Read bytes waiting if data is not found
  long i = 0;
  long j = 0;
  for (i = 0; i < len;)
  {
    int b = Serial.read();
    if (b == -1)
    {
      /*for (j = 0; j < i+1; j++) {
        digitalWrite(ERROR_PIN, HIGH);
        delay(100);
        digitalWrite(ERROR_PIN, LOW);
        delay(100);  
      }
      delay(1000);*/
      continue;
    }
    bytes[i++] = (uint8_t) b;
  }
  /*Serial.print("< ");
  for (i = 0; i < len; i++) {
    Serial.print(bytes[i],HEX);
    Serial.print(" ");
  }
  Serial.println();*/
}
