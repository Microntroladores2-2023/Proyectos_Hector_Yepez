//#include "uarts.h"
#include "def20.h"
#include "crc16.h"
#include "driver/uart.h"


/*
Discrete Coils/Flags

0x01 - Read Coils
0x02 - Read Discrete Inputs
0x05 - Write Single Coil
0x0F - Write Multiple Coils

Registers

0x03 - Read Holding Registers
0x04 - Read Input Registers
0x06 - Write Single Register
0x10 - Write Multiple Registers

*/

#define ID_RTU_LOCAL 1

#define maxInputRegister 256
#define maxHoldingRegister 256

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3            //implemented
#define MB_FC_WRITE_REGISTER 6            //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //no implemented
#define MB_FC_READ_INPUT_REGISTERS 4      //implemented
#define MB_FC_READ_COILS 1                //no implemented 16 bits
#define MB_FC_WRITE_MULTIPLE_COILS 15     //no implemented 16 bits
#define MB_FC_WRITE_COIL 5                //no implemented 16 bits
#define MB_FC_READ_DISCRETE_INPUTS 2      //no implemented 16 bits

//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4

//
// MODBUS  offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4

#define MB_SER_UID 0
#define MB_SER_FUNC 1
#define MB_SER_REGISTER_START 2
#define MB_SER_REGISTER_NUMBER 4

void modbusSerial(uint8_t *ByteArray, uint16_t Length);
