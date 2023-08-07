#include "driver/adc.h"
#include "esp_log.h"
#include "esp_err.h"

#define CH1 ADC1_CHANNEL_4
#define CH2 ADC1_CHANNEL_5

#define NumeroMuestras 60

typedef struct
{
    uint16_t Sensor1;
    uint16_t Sensor2;
} Data_adc;

esp_err_t init_adc(void);