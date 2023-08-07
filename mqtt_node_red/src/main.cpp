#include <Arduino.h>
#include "Wire.h"

#include "math.h"

#include "init_gpio.h"
#include "init_adc.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFi.h>

#define WLAN_SSID "wifihector"
#define WLAN_PASS "123456789"

// MQTT broker settings https://proyectoinfo.mybluemix.net
#define HOST "127.0.0.1"
#define PORT 1883
#define USERNAME "linux"
#define PASSWORD "linux"

// time out loop count
const int timeout = 200;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish adc1 = Adafruit_MQTT_Publish(&mqtt, "adc/datos1");
Adafruit_MQTT_Publish adc2 = Adafruit_MQTT_Publish(&mqtt, "adc/datos2");

void MQTT_connect();

void setup()
{
  delay(1);
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  int i = 0;
  for (; i < timeout; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
      break;
    delay(100);
    Serial.print(".");
  }
  if (i == timeout)
    Serial.println("No Conectado");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  MQTT_connect();
  init_gpio();
  init_adc();
}

void loop()
{
  uint16_t promedio1;
  uint16_t promedio2;

  uint16_t adc_value1 = adc1_get_raw(CH1);
  uint16_t adc_value2 = adc1_get_raw(CH2);

  for (int j = 0; j < NumeroMuestras; j++)
  {
    promedio1 += adc_value1 = adc1_get_raw(CH1);
    promedio2 += adc_value2 = adc1_get_raw(CH2);
  }

  // Calculo Promedio de 100 muestras
  promedio1 /= NumeroMuestras;
  promedio2 /= NumeroMuestras;

  // Establezco límite superior de entrada
  if (promedio1 > 512)
    promedio1 = 512;
  if (promedio2 > 512)
    promedio2 = 512;

  adc1.publish(promedio1);
  vTaskDelay(pdMS_TO_TICKS(1));
  adc2.publish(promedio2);

  Serial.print(promedio1);
  Serial.print("\t");
  Serial.print(promedio1);
  Serial.print("\t");

  vTaskDelay(pdMS_TO_TICKS(2000));
}

void MQTT_connect()
{
  int8_t ret;
  if (mqtt.connected())
  {
    gpio_set_level(LED2, 1);
    return;
  }
  gpio_set_level(LED2, 0);
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 10;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 1 second...");
    mqtt.disconnect();
    delay(1000);
    retries--;
    if (retries == 0)
      Serial.println("No Conectado");
  }
  Serial.println("MQTT Connected!");
}