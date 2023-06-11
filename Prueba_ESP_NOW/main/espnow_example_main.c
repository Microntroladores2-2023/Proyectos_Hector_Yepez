#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "driver/gpio.h"

#define ESP_CHANNEL 1
#define LED2 GPIO_NUM_2
#define B1 GPIO_NUM_0

TaskHandle_t xHandle;

//**********DECLARACIÓN DE VARIABLES GLOBALES*************

uint16_t tiempo_Led = 1000; // variable global
char BUFF[32];
uint8_t ON = 0;

//**********DIRECCIÓN MAC DEL RESPONDER************************

static uint8_t peer_mac[ESP_NOW_ETH_ALEN] = {0x94, 0xe6, 0x86, 0x01, 0xee, 0xe8};

static char *TAG = "esp_now_init";

//***************PROTOTIPO DE FUNCIONES************************

void Blink(void *pvParameters);
void Boton(void *pvParameters);

//************FUNCIÓN DE INICIALIZACIÓN DE GPIOS***************

static esp_err_t init_gpio(void)
{
    gpio_reset_pin(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(B1);
    gpio_set_direction(B1, GPIO_MODE_INPUT);

    ESP_LOGI(TAG, "gpio init completed");
    return ESP_OK;
}

//************FUNCIÓN DE INICIALIZACIÓN DE WIFI********************

static esp_err_t init_wifi(void)
{

    wifi_init_config_t wifi_inti_config = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    nvs_flash_init();
    esp_wifi_init(&wifi_inti_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    esp_wifi_start();

    ESP_LOGI(TAG, "wifi init completed");
    return ESP_OK;
}

//*****************FUNCIONES DE CALLBACK ESPNOW****************************

void recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
    ESP_LOGI(TAG, "Data recived " MACSTR " %s", MAC2STR(esp_now_info->src_addr), data);
}

void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status == ESP_NOW_SEND_SUCCESS)
    {
        ESP_LOGI(TAG, "ESP_NOW_SEND_SUCCESS");
    }
    else
    {
        ESP_LOGW(TAG, "ESP_NOW_SEND_FAIL");
    }
}

//************FUNCIÓN DE INICIALIZACIÓN DE ESPNOW*******************

static esp_err_t init_esp_now(void)
{
    esp_now_init();
    esp_now_register_recv_cb(recv_cb);
    esp_now_register_send_cb(send_cb);

    ESP_LOGI(TAG, "esp now init completed");
    return ESP_OK;
}

//******************FUNCIÓN DE REGISTRO ESPNOW************************

static esp_err_t register_peer(uint8_t *peer_addr)
{
    esp_now_peer_info_t esp_now_peer_info = {};
    memcpy(esp_now_peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    esp_now_peer_info.channel = ESP_CHANNEL;
    esp_now_peer_info.ifidx = ESP_IF_WIFI_STA;
    esp_now_add_peer(&esp_now_peer_info);

    return ESP_OK;
}

//************FUNCIÓN DE ENVIO DE DATOS POR ESPNOW***************

static esp_err_t esp_now_send_data(const uint8_t *peer_addr, const uint8_t *data, size_t len)
{
    esp_now_send(peer_addr, data, len);
    return ESP_OK;
}

//************************FUNCIÓN MAIN***************************

void app_main(void)
{
    ESP_ERROR_CHECK(init_wifi());
    ESP_ERROR_CHECK(init_esp_now());
    ESP_ERROR_CHECK(register_peer(peer_mac));
    ESP_ERROR_CHECK(init_gpio());

    xTaskCreatePinnedToCore(Boton, "Boton", 1024 * 10, NULL, 12, NULL, 0);
    xTaskCreatePinnedToCore(Blink, "Parpadeo", 1024 * 10, NULL, 1, &xHandle, 0);
}

//*********************FUNCIÓN DE TAREAS*************************

void Boton(void *pvParameters)
{

    enum ESTADOS
    {
        velocidad_lenta,
        velocidad_rapida,
        suspensio_BLINK,
        resumen_BLINK,
        velocidad_media
    };

    uint8_t estado_maquina = velocidad_lenta;

    while (1)
    {
        // presiona boton y suelta
        while (gpio_get_level(B1) == 0)
            vTaskDelay(20 / portTICK_PERIOD_MS);

        while (gpio_get_level(B1) == 1)
            vTaskDelay(20 / portTICK_PERIOD_MS);

        switch (estado_maquina)
        {
        case velocidad_rapida:
            tiempo_Led = 100;
            estado_maquina = velocidad_lenta;
            break;

        case velocidad_lenta:
            tiempo_Led = 2000;
            estado_maquina = suspensio_BLINK;
            break;

        case suspensio_BLINK:
            vTaskSuspend(xHandle);
            estado_maquina = resumen_BLINK;
            break;

        case resumen_BLINK:
            vTaskResume(xHandle);
            tiempo_Led = 50;
            estado_maquina = velocidad_media;
            break;

        case velocidad_media:
            tiempo_Led = 500;
            estado_maquina = velocidad_rapida;
            break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void Blink(void *pvParameters)
{

    while (1)
    {
        ON = !ON;
        snprintf(BUFF, sizeof(BUFF), "%u|%u", ON, tiempo_Led);  // Arreglo cadena de caracteres en forma "ON|tiempo_led"
        esp_now_send_data(peer_mac, (const uint8_t *)BUFF, 32); // Envía cadena en forma "ON|tiempo_led"
        gpio_set_level(LED2, ON);
        vTaskDelay(tiempo_Led / portTICK_PERIOD_MS);
    }
}
