#include <Arduino.h>
#include "uarts.h"
#include "tareas.h"
#include "adcs.h"
#include "coils.h"

void setup()
{
  initUART0(); //inicializo uart0
  init_adc(); //inicializo ADCs
  init_coils(); //inicializo salidas
  xTaskCreatePinnedToCore(TareaEntradaDatos, "Tarea_para_entrada1", 1024 * 5, NULL, 12, NULL, 1); //creo tarea1
}

void loop()
{
}