#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "rom/ets_sys.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "init_adc.h"
#include "init_gpio.h"
#include "connect_wifi.h"

static const char *TAG = "MQTT_EXAMPLE";


#define MQTT_PUB_ADC_D1 "esp32/adc/datos1"
#define MQTT_PUB_ADC_D2 "esp32/adc/datos2"

uint32_t MQTT_CONNECTED = 0;

#define ITEM_SIZE sizeof(Data_adc)
#define QUEUE_LENGTH 1

QueueHandle_t xQueue;

void mqtt_app_start(void);

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, (int)event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        gpio_set_level(LED2, 1);
        MQTT_CONNECTED = 1;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        gpio_set_level(LED2, 0);
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t client = NULL;
void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "STARTING MQTT");

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://127.0.0.1:1883",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void ADC_Publisher_task(void *pvParameter)
{
    Data_adc DatosRx;

    while (1)
    {
        if (xQueueReceive(xQueue, &DatosRx, portMAX_DELAY) == pdPASS)
        {

            char datos1[12];
            sprintf(datos1, "%u", DatosRx.Sensor1);

            char datos2[12];
            sprintf(datos2, "%u", DatosRx.Sensor2);

            printf("ADC1 %u \n", DatosRx.Sensor1);
            printf("ADC2 %u \n", DatosRx.Sensor2);

            if (MQTT_CONNECTED)
            {

                esp_mqtt_client_publish(client, MQTT_PUB_ADC_D1, datos1, 0, 0, 0);
                esp_mqtt_client_publish(client, MQTT_PUB_ADC_D2, datos2, 0, 0, 0);
            }
        }
    }
}

void DataIN(void *pvParameter)
{
    Data_adc DatoxTx;

    uint16_t promedio1 = 0;
    uint16_t promedio2 = 0;

    uint16_t adc_value1 = adc1_get_raw(CH1);
    uint16_t adc_value2 = adc1_get_raw(CH2);
    while (1)
    {
        for (int j = 0; j < NumeroMuestras; j++)
        {
            promedio1 += adc_value1 = adc1_get_raw(CH1);
            promedio2 += adc_value2 = adc1_get_raw(CH2);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        // Calculo Promedio de 100 muestras
        promedio1 /= NumeroMuestras;
        promedio2 /= NumeroMuestras;

        // Establezco límite superior de entrada
        if (promedio1 > 512)
            promedio1 = 512;
        if (promedio2 > 512)
            promedio2 = 512;

        DatoxTx.Sensor1 = promedio1;
        DatoxTx.Sensor2 = promedio2;

        xQueueSend(xQueue, &DatoxTx, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    connect_wifi();

    if (wifi_connect_status)
    {
        init_gpio();
        init_adc();
        xQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

        if (xQueue != NULL)
        {
            xTaskCreatePinnedToCore(ADC_Publisher_task, "DHT_Publisher_task", 1024 * 10, NULL, 1, NULL, 1);
            xTaskCreatePinnedToCore(DataIN, "Entrada de datos ADC", 1024 * 10, NULL, 1, NULL, 0);
        }
        else
        {
            ESP_LOGW(TAG, "Tareas no creadas");
        }
    }
}