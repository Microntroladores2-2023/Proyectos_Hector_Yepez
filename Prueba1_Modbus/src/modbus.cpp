#include "modbus.h"
#include "coils.h"

#define bitRead(valor, bit) (((valor) >> (bit)) & 0x01)
#define bitSet(valor, bit) ((valor |= (1UL << (bit))))
#define bitClear(valor, bit) ((valor) &= ~(1UL << (bit)))
#define bitWrite(valor, bit, bitvalue) ((bitvalue) ? bitSet(valor, bit) : bitClear(valor, bit))

// uint8_t ByteArray[260]; // buffer de recepcion de los datos recibidos de los clientes
UINT16_VAL MBHoldingRegister[maxHoldingRegister];
UINT16_VAL MBInputRegister[maxInputRegister];
UINT16_VAL MBCoils;
UINT16_VAL MBDiscreteInputs;

//**************************************
//************* Funciones **************
//**************************************

void modbusSerial(uint8_t *ByteArray, uint16_t Length)
{
    uint8_t byteFN = ByteArray[MB_SER_FUNC]; // maquina de estado
    UINT16_VAL Start;
    UINT16_VAL WordDataLength;

    UINT16_VAL CRC;

    if ((CRC16(ByteArray, Length) == 0) && (ByteArray[MB_SER_UID] == ID_RTU_LOCAL))
    {

        switch (byteFN)
        {
        case MB_FC_NONE:

            break;

        case MB_FC_READ_COILS: // 01 Read Coils
            ByteArray[2] = 2;

            ByteArray[3] = MBCoils.byte.LB;
            ByteArray[4] = MBCoils.byte.HB;

            CRC.Val = CRC16(ByteArray, 5);

            ByteArray[5] = (CRC.byte.LB);
            ByteArray[6] = (CRC.byte.HB);

            uart_write_bytes(UART_NUM_0, (const char *)ByteArray, 8);
            break;

        case MB_FC_READ_DISCRETE_INPUTS: // funcion 02: Read Discrete Inputs

            break;

        case MB_FC_READ_REGISTERS: // 03 Read Holding Registers

            // direccion de cominzo Modbus
            Start.byte.HB = ByteArray[2];
            Start.byte.LB = ByteArray[3];

            // numero de datos
            WordDataLength.byte.HB = ByteArray[4];
            WordDataLength.byte.LB = ByteArray[5];

            // numero de bytes de datos de respuesta
            ByteArray[2] = WordDataLength.Val * 2;

            for (uint16_t i = 0; i < WordDataLength.Val; i++) // datos de respuesta
            {
                ByteArray[3 + i * 2] = MBHoldingRegister[i + Start.Val].byte.HB;
                ByteArray[4 + i * 2] = MBHoldingRegister[i + Start.Val].byte.LB;
            }

            // CRC
            CRC.Val = CRC16(ByteArray, ByteArray[2] + 3);
            ByteArray[ByteArray[2] + 3] = (CRC.byte.LB);
            ByteArray[ByteArray[2] + 4] = (CRC.byte.HB);

            // rs485(ByteArray, ByteArray[2] + 5);

            uart_write_bytes(UART_NUM_0, (const char *)ByteArray, ByteArray[2] + 5);

            break;

        case MB_FC_READ_INPUT_REGISTERS: // 04 Read Input Registers Registers
                                         // direccion de cominzo Modbus
            Start.byte.HB = ByteArray[2];
            Start.byte.LB = ByteArray[3];

            // numero de datos
            WordDataLength.byte.HB = ByteArray[4];
            WordDataLength.byte.LB = ByteArray[5];

            // numero de bytes de datos de respuesta
            ByteArray[2] = WordDataLength.Val * 2;

            for (uint16_t i = 0; i < WordDataLength.Val; i++) // datos de respuesta
            {
                ByteArray[3 + i * 2] = MBInputRegister[i + Start.Val].byte.HB;
                ByteArray[4 + i * 2] = MBInputRegister[i + Start.Val].byte.LB;
            }

            // CRC
            CRC.Val = CRC16(ByteArray, ByteArray[2] + 3);
            ByteArray[ByteArray[2] + 3] = (CRC.byte.LB);
            ByteArray[ByteArray[2] + 4] = (CRC.byte.HB);

            // rs485(ByteArray, ByteArray[2] + 5);

            uart_write_bytes(UART_NUM_0, (const char *)ByteArray, ByteArray[2] + 5);

            break;

        case MB_FC_WRITE_COIL: // 05 Write COIL
           
            uart_write_bytes(UART_NUM_0, (const char *)ByteArray, 8);

            // direccion de coil

            Start.byte.HB = ByteArray[2];
            Start.byte.LB = ByteArray[3];

            // Comando encender o apagar Coil

            WordDataLength.byte.HB = ByteArray[4];
            WordDataLength.byte.LB = ByteArray[5];

            // comprueba una dirección entre 0 y 15

            switch (Start.Val)
            {
                // Dirección coil 1
            case 0:
                // Comprueba si comando es ON
                if (WordDataLength.Val == 0xFF00)
                {
                    bitWrite(MBCoils.Val, 0, 1);
                }
                // Si no comando OFF
                else
                {
                    bitWrite(MBCoils.Val, 0, 0);
                }
                break;

                // Dirección coil 2

            case 1:
                // Comprueba si comando es ON
                if (WordDataLength.Val == 0xFF00)
                {
                    bitWrite(MBCoils.Val, 1, 1);
                }
                // Si no comando OFF
                else
                {
                    bitWrite(MBCoils.Val, 1, 0);
                }
                break;

                // Dirección coil 3

            case 2:
                // Comprueba si comando es ON
                if (WordDataLength.Val == 0xFF00)
                {
                    bitWrite(MBCoils.Val, 2, 1);
                }
                // Si no comando OFF
                else
                {
                    bitWrite(MBCoils.Val, 2, 0);
                }
                break;

                // Dirección coil 4

            case 3:
                // Comprueba si comando es ON
                if (WordDataLength.Val == 0xFF00)
                {
                    bitWrite(MBCoils.Val, 3, 1);
                }
                // Si no comando OFF
                else
                {
                    bitWrite(MBCoils.Val, 3, 0);
                }
                break;

            default:
                break;
            }

            break;

        case MB_FC_WRITE_REGISTER: // 06 Write Holding Register

            // direccion de cominzo Modbus
            Start.byte.HB = ByteArray[2];
            Start.byte.LB = ByteArray[3];

            MBHoldingRegister[Start.Val].byte.HB = ByteArray[4];
            MBHoldingRegister[Start.Val].byte.LB = ByteArray[5];

            // rs485(ByteArray, 8);

            uart_write_bytes(UART_NUM_0, (const char *)ByteArray, 8);

            break;

        case MB_FC_WRITE_MULTIPLE_COILS: // 15 Write Coils

            break;

        case MB_FC_WRITE_MULTIPLE_REGISTERS: // 16 Write Holding Registers

            break;
        }
    }
}