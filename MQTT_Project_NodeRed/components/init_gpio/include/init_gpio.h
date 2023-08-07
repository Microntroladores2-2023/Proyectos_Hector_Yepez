#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#define OUT1 GPIO_NUM_19
#define OUT2 GPIO_NUM_17
#define OUT3 GPIO_NUM_18
#define OUT4 GPIO_NUM_16
#define LED2 GPIO_NUM_2

#define IN1 GPIO_NUM_32
#define IN2 GPIO_NUM_33
esp_err_t init_gpio(void);


typedef struct
{
    bool Sensor1;
    bool Sensor2;
    bool F_LEFT;
    bool F_RIGHT;
} Data_IN;