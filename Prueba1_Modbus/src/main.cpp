#include <Arduino.h>
#include "uarts.h"
#include "tareas.h"

void setup()
{
  initUART0();
  xTaskCreatePinnedToCore(TareaEntradaDatos, "Tarea_para_entrada1", 1024 * 2, NULL, 12, NULL, 1);
}

void loop()
{
}