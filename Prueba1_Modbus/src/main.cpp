#include <Arduino.h>
#include "uarts.h"
#include "tareas.h"
#include "adcs.h"
#include "coils.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int RSSI_THRESHOLD = -65;
int scanTime = 1; // In seconds
BLEScan *pBLEScan;
int rssi;

void setup()
{
  initUART0();                                                                                    // inicializo uart0
  init_adc();                                                                                     // inicializo ADCs
  init_coils();                                                                                   // inicializo salidas
  xTaskCreatePinnedToCore(TareaEntradaDatos, "Tarea_para_entrada1", 1024 * 5, NULL, 12, NULL, 1); // creo tarea1

  BLEDevice::init("");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // create new scan
  pBLEScan->setActiveScan(true);   // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);      // set Scan interval
  pBLEScan->setWindow(99);         // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  for (int i = 0; i < foundDevices.getCount(); i++)
  {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    rssi = device.getRSSI();
    if (rssi > RSSI_THRESHOLD) gpio_set_level(LED1, 1);
    else gpio_set_level(LED1, 0);
  }
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
}