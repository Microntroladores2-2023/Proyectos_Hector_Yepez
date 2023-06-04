#include "modbusTCPIP.h"
#include "tareas.h"
#include "adcs.h"
#include "coils.h"


void setup()
{
  init_coils(); // inicializo salidas
  init_adc();   // inicializo ADCs
  setupWiFiServer();
  xTaskCreatePinnedToCore(TareaEntradaDatos, "Tarea_para_entrada1", 1024 * 5, NULL, 12, NULL, 1); // creo tarea1
}

void loop()
{
}
