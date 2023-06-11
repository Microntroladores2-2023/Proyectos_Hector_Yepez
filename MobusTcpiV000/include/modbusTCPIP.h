#include <Arduino.h>
#include "def20.h"
#include <WiFi.h>


//**************************************
//********* Requeridas MODBUS **********
//**************************************

#define maxInputRegister 256
#define maxHoldingRegister 256

#define ID_RTU_LOCAL 1

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3            //no implemented
#define MB_FC_WRITE_REGISTER 6            //no implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //no implemented

#define MB_FC_READ_INPUT_REGISTERS 4      //no implemented
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
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_REGISTER_START 8
#define MB_TCP_REGISTER_NUMBER 10


void modbusTCPIP(uint8_t *ByteArray, uint8_t Length);
void TaskWiFiServer(void *pvParameters);
void setupWiFiServer();

