#include "modbusTCPIP.h"

UINT16_VAL MBHoldingRegister[maxHoldingRegister];
UINT16_VAL MBInputRegister[maxInputRegister];
UINT16_VAL MBCoils;
UINT16_VAL MBDiscreteInputs;
int ModbusTCP_port = 502;

WiFiServer server(ModbusTCP_port);

WiFiClient client;

// Network information
const char *ssid = "WIFI_HOME_VV";
const char *password = "C4s4VV001";

//**************************************
//********* Init WiFi Server ***********
//**************************************
void setupWiFiServer()
{

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Desactiva la suspensión de wifi en modo STA para mejorar la velocidad de respuesta
  WiFi.begin(ssid, password);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  xTaskCreatePinnedToCore(TaskWiFiServer, "WiFiServer", 1024 * 5, NULL, 2, NULL, 1);
}

//**************************************
//*************** TASKs ****************
//**************************************

void modbusTCPIP(uint8_t *ByteArray, uint8_t Length)
{

  int MessageLength;
  uint8_t byteFN = ByteArray[MB_TCP_FUNC];

  UINT16_VAL longData;
  UINT16_VAL direccionComienzo;

  // Handle request
  if (ByteArray[MB_TCP_UID] == ID_RTU_LOCAL)
  {
    switch (byteFN)
    {
    case MB_FC_NONE:

      break;

    case MB_FC_READ_COILS: // 01 Read Coils

      ByteArray[8] = 2;

      ByteArray[9] = MBCoils.byte.LB;
      ByteArray[10] = MBCoils.byte.HB;

      client.write((const uint8_t *)ByteArray, 11);

      byteFN = MB_FC_NONE;

      break;

    case MB_FC_READ_DISCRETE_INPUTS: // 02 Read Discrete Inputs

      client.write((const uint8_t *)ByteArray, MessageLength);

      byteFN = MB_FC_NONE;

      break;

    case MB_FC_READ_REGISTERS: // 03 Read Holding Registers

      // numero de datos
      longData.byte.HB = ByteArray[10];
      longData.byte.LB = ByteArray[11];

      // direccion de cominzo Modbus
      direccionComienzo.byte.HB = ByteArray[8];
      direccionComienzo.byte.LB = ByteArray[9];

      ByteArray[8] = longData.Val * 2; // numero de bytes de datos de respuesta
      ByteArray[5] = ByteArray[8] + 9;
      for (uint16_t i = 0; i < longData.Val; i++) // datos de respuesta
      {
        ByteArray[9 + i * 2] = MBHoldingRegister[i + direccionComienzo.Val].byte.HB;
        ByteArray[10 + i * 2] = MBHoldingRegister[i + direccionComienzo.Val].byte.LB;
      }

      // respuesta ModbusTCPIP

      client.write((const uint8_t *)ByteArray, ByteArray[8] + 9);

      byteFN = MB_FC_NONE;

      break;

    case MB_FC_READ_INPUT_REGISTERS: // 04 Read Input Registers Registers

      // numero de datos
      longData.byte.HB = ByteArray[10];
      longData.byte.LB = ByteArray[11];

      // direccion de cominzo Modbus
      direccionComienzo.byte.HB = ByteArray[8];
      direccionComienzo.byte.LB = ByteArray[9];

      ByteArray[8] = longData.Val * 2; // numero de bytes de datos de respuesta
      ByteArray[5] = ByteArray[8] + 9;
      for (uint16_t i = 0; i < longData.Val; i++) // datos de respuesta
      {
        ByteArray[9 + i * 2] = MBInputRegister[i + direccionComienzo.Val].byte.HB;
        ByteArray[10 + i * 2] = MBInputRegister[i + direccionComienzo.Val].byte.LB;
      }

      client.write((const uint8_t *)ByteArray, ByteArray[8] + 9);

      byteFN = MB_FC_NONE;

      break;

    case MB_FC_WRITE_COIL: // 05 Write COIL

      client.write((const uint8_t *)ByteArray, MessageLength);

      // Bytes de comando
      longData.byte.HB = ByteArray[10];
      longData.byte.LB = ByteArray[11];

      // direccion de coil
      direccionComienzo.byte.HB = ByteArray[8];
      direccionComienzo.byte.LB = ByteArray[9];

      // comprueba una dirección entre 0 y 15

      switch (direccionComienzo.Val)
      {
        // Dirección coil 1
      case 0:
        // Comprueba si comando es ON
        if (longData.Val == 0xFF00)
        {
          bitWrite(MBCoils.Val, 0, 1);
        }
        // Si no comando OFF
        else
        {
          bitWrite(MBCoils.Val, 0, 0);
        }
        break;

        // Dirección coil 2

      case 1:
        // Comprueba si comando es ON
        if (longData.Val == 0xFF00)
        {
          bitWrite(MBCoils.Val, 1, 1);
        }
        // Si no comando OFF
        else
        {
          bitWrite(MBCoils.Val, 1, 0);
        }
        break;

        // Dirección coil 3

      case 2:
        // Comprueba si comando es ON
        if (longData.Val == 0xFF00)
        {
          bitWrite(MBCoils.Val, 2, 1);
        }
        // Si no comando OFF
        else
        {
          bitWrite(MBCoils.Val, 2, 0);
        }
        break;

        // Dirección coil 4

      case 3:
        // Comprueba si comando es ON
        if (longData.Val == 0xFF00)
        {
          bitWrite(MBCoils.Val, 3, 1);
        }
        // Si no comando OFF
        else
        {
          bitWrite(MBCoils.Val, 3, 0);
        }
        break;

      default:
        break;
      }

      byteFN = MB_FC_NONE;
      break;

    case MB_FC_WRITE_REGISTER: // 06 Write Holding Register

      // direccion de cominzo Modbus
      direccionComienzo.byte.HB = ByteArray[8];
      direccionComienzo.byte.LB = ByteArray[9];

      MBHoldingRegister[direccionComienzo.Val].byte.HB = ByteArray[10];
      MBHoldingRegister[direccionComienzo.Val].byte.LB = ByteArray[11];

      client.write((const uint8_t *)ByteArray, 12);

      byteFN = MB_FC_NONE;
      break;

    case MB_FC_WRITE_MULTIPLE_COILS: // 15 Write Coils

      // client.write((const uint8_t *)ByteArray, MessageLength);

      byteFN = MB_FC_NONE;
      break;

    case MB_FC_WRITE_MULTIPLE_REGISTERS: // 16 Write Holding Registers

      // client.write((const uint8_t *)ByteArray, MessageLength);

      byteFN = MB_FC_NONE;
      break;

    } // fin de switch
  }
} // fin funcion

void TaskWiFiServer(void *pvParameters)
{
  server.begin();
  uint8_t ByteArray[255]; // buffer de recepcion de los datos recibidos de los clientes
  uint8_t nDatosRecibidos;

  for (;;)
  {

    // Check if a client has connected
    client = server.available(); // listen for incoming clients
    if (client)                  // if you get a client
    {
      while (client.connected()) // loop while the client's connected
      {
        if (client.available())
        {
          nDatosRecibidos = 0;

          while (client.available())
          {
            ByteArray[nDatosRecibidos++] = client.read();
          }
          // termina bytes to read from the client

          // for (int i = 0; i < nDatosRecibidos; i++) printf("%c", ByteArray[i]);  //imprime datos Recibidos

          client.flush(); // Discard any bytes that have been written to the client but not yet read.
          if (nDatosRecibidos > 0)
            modbusTCPIP(ByteArray, nDatosRecibidos);
        }
      }
      client.stop();
    } // fin cliente

    vTaskDelay(100);

  } // fin for
} // fin Task
