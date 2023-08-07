#include <stdio.h>
#include "init_gpio.h"

static char *tag = "GPIO";

esp_err_t init_gpio(void)
{

    gpio_reset_pin(OUT1);
    gpio_set_direction(OUT1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(OUT2);
    gpio_set_direction(OUT2, GPIO_MODE_OUTPUT);
    gpio_reset_pin(OUT3);
    gpio_set_direction(OUT3, GPIO_MODE_OUTPUT);
    gpio_reset_pin(OUT4);
    gpio_set_direction(OUT4, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
/*
    gpio_reset_pin(IN1);
    gpio_set_direction(IN1, GPIO_MODE_INPUT);

    gpio_reset_pin(IN2);
    gpio_set_direction(IN2, GPIO_MODE_INPUT);
*/

    gpio_set_level(OUT1, 0);
    gpio_set_level(OUT2, 0);
    gpio_set_level(OUT3, 0);
    gpio_set_level(OUT4, 0);

    ESP_LOGI(tag, "Init gpio completed");

    return ESP_OK;
}
