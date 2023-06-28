#include <Arduino.h>
#include "adcs.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/queue.h"
#include "ultrasonic.h"

#define MAX_DISTANCE_CM 500 // 5m max

#define TRIGGER_GPIO GPIO_NUM_33
#define ECHO_GPIO GPIO_NUM_25

// Queue parameters
#define QUEUE_LENGTH 3
#define ITEM_SIZE sizeof(Datos)

// Queue handle
QueueHandle_t xQueue;

typedef struct
{
  float valuef;
  uint32_t valadc1;
  uint32_t valadc2;
  uint32_t valadc3;
} Datos;

TaskHandle_t xHandle_http_task = NULL;

TaskHandle_t xHandle_entrada_datos1 = NULL;
TaskHandle_t xHandle_entrada_datos2 = NULL;

uint32_t promedio1 = 0;
uint32_t promedio2 = 0;
uint32_t promedio3 = 0;
float distance;

void DatosADC(void *pvParameters)
{

  uint32_t adc_value1 = adc1_get_raw(CH1);
  uint32_t adc_value2 = adc1_get_raw(CH2);
  uint32_t adc_value3 = adc1_get_raw(CH3);

  while (1)
  {
    // Iteración valores ADCs
    for (int j = 0; j < NumeroMuestras; j++)
    {

      promedio1 += adc_value1 = adc1_get_raw(CH1);
      promedio2 += adc_value2 = adc1_get_raw(CH2);
      promedio3 += adc_value3 = adc1_get_raw(CH3);
    }

    vTaskDelay(20 / portTICK_PERIOD_MS);

    // Calculo Promedio de 100 muestras
    promedio1 /= NumeroMuestras;
    promedio2 /= NumeroMuestras;
    promedio3 /= NumeroMuestras;

    // Establezco límite superior de entrada
    if (promedio1 > 4095)
      promedio1 = 4095;
    if (promedio2 > 4095)
      promedio2 = 4095;
    if (promedio3 > 4095)
      promedio3 = 4095;
    /*
        Serial.printf("promedio 3: ");
        Serial.println(promedio3);
           Serial.printf("promedio 2: ");
           Serial.println(promedio2);
           Serial.printf("promedio 1: ");
           Serial.println(promedio1);
       */

    Datos datosTx;
    datosTx.valadc1 = promedio1;
    datosTx.valadc2 = promedio2;
    datosTx.valadc3 = promedio3;
    datosTx.valuef = distance;

    xQueueSend(xQueue, &datosTx, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void DatosFloat(void *pvParameters)
{
  ultrasonic_sensor_t sensor = {
      .trigger_pin = TRIGGER_GPIO,
      .echo_pin = ECHO_GPIO};

  ultrasonic_init(&sensor);

  while (1)
  {
    esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
    if (res != ESP_OK)
    {
      printf("Error %d: ", res);
      switch (res)
      {
      case ESP_ERR_ULTRASONIC_PING:
        printf("Cannot ping (device is in invalid state)\n");
        break;
      case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
        printf("Ping timeout (no device found)\n");
        break;
      case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
        printf("Echo timeout (i.e. distance too big)\n");
        break;
      default:
        printf("%s\n", esp_err_to_name(res));
      }
    }
    // else printf("Distance: %0.02f cm\n", distance * 100);

    Datos datosTx;
    datosTx.valadc1 = promedio1;
    datosTx.valadc2 = promedio2;
    datosTx.valadc3 = promedio3;
    datosTx.valuef = distance;

    xQueueSend(xQueue, &datosTx, portMAX_DELAY);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void SendHTTP(void *pvParameters)
{
  // int32_t receivedItem;
  Datos datosRx;

  // Scada Vemetris en Digital Ocean
  String ScadaVemetris = "http://137.184.178.17:21486/httpds?__device=Sensores_Hector";

  while (1)
  {
    // Receive data from the queue

    if (xQueueReceive(xQueue, &datosRx, portMAX_DELAY) == pdPASS)
    {
      HTTPClient http;

      String dato1 = String(datosRx.valadc1);

      String dato2 = String(datosRx.valadc2);

      String dato3 = String(datosRx.valadc3);

      String dato4 = String(datosRx.valuef);

      printf("Distance: %s m\n", dato4);
      printf("Promedio3: %s \n", dato3);

      String Trama = ScadaVemetris + "&rssi=" + WiFi.RSSI() + "&dato1=" + dato1 + "&dato2=" + dato2 + "&dato3=" + dato3 + "&dato4=" + dato4;

      http.begin(Trama);         // Iniciar conexión
      int httpCode = http.GET(); // Realizar petición

      if (httpCode > 0)
      {
        String payload = http.getString(); // Obtener respuesta
        Serial.println(httpCode);          // Si el codigo es 200, se realizo bien
        Serial.println(payload);           // Mostrar respuesta por serial
      }
      else
      {
        Serial.println("Error enviadno la trama");
      }
      http.end(); // Se libera el cliente

      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}

void initWiFi(void)
{

  const char *ssid = "wifihector";
  const char *password = "123456789";

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.println("Configurando Red Wi-Fi");
  }
}

void setup()
{
  init_adc();
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  initWiFi();
  //  Create the queue
  xQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

  if (xQueue != NULL)
  {
    xTaskCreatePinnedToCore(DatosADC, "Entrada Datos ADC1", 1024 * 4, NULL, 1, &xHandle_entrada_datos1, 1);

    xTaskCreatePinnedToCore(DatosFloat, "Entrada Datos ultrasonido", 1024 * 4, NULL, 1, &xHandle_entrada_datos2, 1);

    xTaskCreatePinnedToCore(SendHTTP, "Envio de datos por HTTP", 1024 * 4, NULL, 2, &xHandle_http_task, 0);
  }
  else
  {
    Serial.println("Tareas no creadas");
  }
}

void loop()
{
}
