#include <Arduino.h>
#include "adcs.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/queue.h"

void setup() {
init_adc();
Serial.begin(115200);
vTaskDelay(1000 / portTICK_PERIOD_MS);
initWiFi();
xTaskCreatePinnedToCore(TaskEntradaDatos, "EntradaDatos", 4096, NULL, 2, &xHandle_entrada_datos, 1);
xTaskCreatePinnedToCore(TaskHTTP, "HTTPcliente", 4096, NULL, 4, &xHandle_http_task, 1);
}

void initWiFi(void) {

  const char* ssid = "wifihector";
  const char* password = "123456789";

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.println("Configurando Red Wi-Fi");
  }
}

void loop() {
}
