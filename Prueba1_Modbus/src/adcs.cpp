#include "adcs.h"


static esp_adc_cal_characteristics_t adc1_chars;

void init_adc()
{

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc1_chars);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(CH1, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(CH2, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(CH3, ADC_ATTEN_DB_11);
}