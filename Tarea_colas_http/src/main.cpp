#include <Arduino.h>
#include "adcs.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/queue.h"

// Queue parameters
#define QUEUE_LENGTH 5
#define ITEM_SIZE sizeof(Datos)
#define SENSOR_GPIO GPIO_NUM_33
// Queue handle
QueueHandle_t xQueue;

typedef struct
{
  int id;
  float valuef;
  uint32_t valadc1;
  uint32_t valadc2;
  uint32_t valadc3;
} Datos;

TaskHandle_t xHandle_http_task = NULL;
TaskHandle_t xHandle_entrada_datos = NULL;

void EntradasADC(void *pvParameters)
{
  uint32_t promedio1 = 0;
  uint32_t promedio2 = 0;
  uint32_t promedio3 = 0;

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

    Datos datosTx;
    datosTx.id = 1;
    datosTx.valadc1 = promedio1;
    datosTx.valadc2 = promedio2;
    datosTx.valadc3 = promedio3;

    xQueueSend(xQueue, &datosTx, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void EntradasFloat(void *pvParameters)
{
  gpio_reset_pin(SENSOR_GPIO);
  gpio_set_direction(SENSOR_GPIO, GPIO_MODE_INPUT_OUTPUT);
  while (1)
  {
    gpio_set_level(SENSOR_GPIO, 0);
    ets_delay_us(2);
    gpio_set_level(SENSOR_GPIO, 1);
    ets_delay_us(10);
    gpio_set_level(SENSOR_GPIO, 1);

    // Espera a que el eco del sensor sea detectado
    uint32_t tiempo_inicio = 0;
    uint32_t tiempo_final = 0;

    while (gpio_get_level(SENSOR_GPIO) == 0)
    {
      tiempo_inicio = esp_timer_get_time();
    }
    while (gpio_get_level(SENSOR_GPIO) == 1)
    {
      tiempo_final = esp_timer_get_time();
    }
    // Calcular la distancia en centímetros
    uint32_t duracion = tiempo_final - tiempo_inicio;
    float distancia = duracion * 0.034 / 2;

    Datos datosTx;
    datosTx.id = 1;
    datosTx.valuef = distancia;
    Serial.println(distancia);
    xQueueSend(xQueue, &datosTx, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  esp_timer_get_time();
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

      vTaskDelay(5000 / portTICK_PERIOD_MS);
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
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // Create the queue
  xQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

  if (xQueue != NULL)
  {
    xTaskCreatePinnedToCore(EntradasADC, "Entrada Datos ADC1", configMINIMAL_STACK_SIZE, NULL, 2, &xHandle_entrada_datos, 1);

    xTaskCreatePinnedToCore(EntradasFloat, "Entrada de datos float", configMINIMAL_STACK_SIZE, NULL, 4, &xHandle_entrada_datos, 1);

    xTaskCreatePinnedToCore(SendHTTP, "Envio de datos por HTTP", configMINIMAL_STACK_SIZE, NULL, 6, &xHandle_http_task, 1);
  }
  else{
    Serial.println("Tareas no creadas");
  }
}

void loop()
{
}
