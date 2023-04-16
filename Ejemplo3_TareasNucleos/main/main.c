#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Tarea1(void *pvParameters) // Esta es una tarea
{

    while (1) // Una tarea nunca regresará ni saldrá
    {
        printf("Hola Mundo Tarea1 freeRTOS Estoy corriendo en el nucluo = %d\n\r", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void Tarea3(void *pvParameters) // Esta es una tarea
{

    while (1) // Una tarea nunca regresará ni saldrá
    {
        printf("Hola Mundo Tarea3 freeRTOS Estoy corriendo en el nucluo = %d\n\r", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void Tarea2(void *pvParameters) // Esta es una tarea
{

    while (1) // Una tarea nunca regresará ni saldrá
    {
        printf("Hola Mundo Tarea2 freeRTOS Estoy corriendo en el nucluo = %d\n\r", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void Tarea4(void *pvParameters) // Esta es una tarea
{

    while (1) // Una tarea nunca regresará ni saldrá
    {
        printf("Hola Mundo Tarea4 freeRTOS Estoy corriendo en el nucluo = %d\n\r", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{

    xTaskCreatePinnedToCore(Tarea1, "Task1", 1024 * 2, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(Tarea2, "Task2", 1024 * 2, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(Tarea3, "Task3", 1024 * 2, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(Tarea4, "Task4", 1024 * 2, NULL, 4, NULL, 1);
}