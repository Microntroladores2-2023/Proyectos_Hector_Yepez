#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Tarea1(void *pvParameters) // Esta es una tarea
{

    while (1) // Una tarea nunca regresará ni saldrá
    {
        
        printf("Hola Mundo Tarea1 freeRTOS\n\r");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}

void app_main()
{
    xTaskCreatePinnedToCore(Tarea1, "Task1", 1024 * 2, NULL, 1, NULL, 0);
}