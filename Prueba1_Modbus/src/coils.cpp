#include "coils.h"

void init_coils(){

    gpio_reset_pin(C0);
    gpio_set_direction(C0, GPIO_MODE_OUTPUT);
    gpio_reset_pin(C1);
    gpio_set_direction(C1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(C2);
    gpio_set_direction(C2, GPIO_MODE_OUTPUT);

}