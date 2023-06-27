#include "driver/adc.h"
#include "esp_adc_cal.h"

#define CH1 ADC1_CHANNEL_0
#define CH2 ADC1_CHANNEL_4
#define CH3 ADC1_CHANNEL_7


#define NumeroMuestras 100

void init_adc();
