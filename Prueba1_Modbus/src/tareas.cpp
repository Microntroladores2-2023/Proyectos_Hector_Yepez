#include "modbus.h"

extern UINT16_VAL MBHoldingRegister[maxHoldingRegister];
extern UINT16_VAL MBInputRegister[maxInputRegister];
extern UINT16_VAL MBCoils;
extern UINT16_VAL MBDiscreteInputs;

void TareaEntradaDatos(void *Parametro)
{
    int16_t datoInAux1 = -100;
    int16_t datoInAux2 = -20;
    int16_t datoInAux3 = 100;
    int16_t datoInAux4 = 0;
    uint16_t cont1 = 0;

    while (1)
    {
        MBInputRegister[0].Val = datoInAux1++;
        MBInputRegister[1].Val = datoInAux2++;
        MBInputRegister[2].Val = datoInAux3++;
        MBInputRegister[3].Val = datoInAux4++;

        if (cont1++ == 50)
        {
            datoInAux1 = -100;
            datoInAux2 = -20;
            datoInAux3 = 100;
            datoInAux4 = 0;
            cont1 = 0;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}