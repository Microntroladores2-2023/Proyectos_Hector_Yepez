#include "modbus.h"
#include "adcs.h"

extern UINT16_VAL MBHoldingRegister[maxHoldingRegister];
extern UINT16_VAL MBInputRegister[maxInputRegister];
extern UINT16_VAL MBCoils;
extern UINT16_VAL MBDiscreteInputs;

void TareaEntradaDatos(void *Parametro)
{
    int16_t datoInAux1;
    int16_t datoInAux2;
    int16_t datoInAux3;
    //uint16_t cont1 = 0;

    while (1)
    {
        MBInputRegister[0].Val = adc1_get_raw(CH1);
        MBInputRegister[1].Val = adc1_get_raw(CH2);
        MBInputRegister[2].Val = adc1_get_raw(CH3);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}