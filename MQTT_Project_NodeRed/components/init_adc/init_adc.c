#include <stdio.h>
#include "init_adc.h"

static char *tag = "init ADC";

esp_err_t init_adc(void)
{
    adc1_config_width(ADC_WIDTH_BIT_9);
    adc1_config_channel_atten(CH1, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(CH2, ADC_ATTEN_DB_11);

    ESP_LOGI(tag, "init ADC completed");
    return ESP_OK;
}
