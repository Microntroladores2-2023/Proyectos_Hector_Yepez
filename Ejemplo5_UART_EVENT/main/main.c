#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

static QueueHandle_t uart0_queue;

#define tamBUFFER 1024
#define led1 22
#define led2 18
#define led3 4

static const char *tag = "uart0";

void TareaEventosUART0(void *Parametro)
{
    uart_event_t evento;

    uint8_t *datoRX = (uint8_t *)malloc(tamBUFFER);
    while (1)
    {
        if (xQueueReceive(uart0_queue, (void *)&evento, (TickType_t)portMAX_DELAY))
        {
            bzero(datoRX, tamBUFFER);
            switch (evento.type)
            {
            case UART_DATA:

                uart_read_bytes(UART_NUM_0, datoRX, evento.size, pdMS_TO_TICKS(100));
                uart_write_bytes(UART_NUM_0, (const char *)datoRX, evento.size);
                uart_flush(UART_NUM_0);

                ESP_LOGI(tag, "Data recibida: %s", datoRX);

                for (size_t i = 0; i < evento.size - 2; i++)
                {
                    char value = datoRX[i];

                    switch (value)
                    {

                    case '1':
                        gpio_set_level(led1, 1);
                        gpio_set_level(led2, 0);
                        gpio_set_level(led3, 0);
                        break;

                    case '2':
                        gpio_set_level(led1, 0);
                        gpio_set_level(led2, 1);
                        gpio_set_level(led3, 0);
                        break;

                    case '3':

                        gpio_set_level(led1, 0);
                        gpio_set_level(led2, 0);
                        gpio_set_level(led3, 1);
                        break;

                    default:

                        gpio_set_level(led1, 0);
                        gpio_set_level(led2, 0);
                        gpio_set_level(led3, 0);

                        break;
                    }
                }

                break;

            default:
                break;
            }
        }
    }
}

void init_uart0()
{
    uart_config_t configUART0 = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_0, &configUART0);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, tamBUFFER * 2, tamBUFFER * 2, 20, &uart0_queue, 0);

    xTaskCreatePinnedToCore(TareaEventosUART0, "Tarea_para_UART0", 1024 * 2, NULL, 12, NULL, 1);

    ESP_LOGI(tag, "uart0 inicializado");
}

static void init_led()
{
    gpio_reset_pin(led1);
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(led2);
    gpio_set_direction(led2, GPIO_MODE_OUTPUT);
    gpio_reset_pin(led3);
    gpio_set_direction(led3, GPIO_MODE_OUTPUT);

    ESP_LOGI(tag, "salidas inicializadas");
}

void app_main(void)
{
    init_led();
    init_uart0();
}
