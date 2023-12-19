#include <can.h>
#include <mcp2515.h>

#include <CanHacker.h>
#include <CanHackerLineReader.h>
#include <lib.h>

#include <SPI.h>
#include <SoftwareSerial.h>
//----------------------------------------------------------------------
//------ESP32 CAN BUS setting ---------------------------------------
const int SPI_CS_PIN = 5;
const int INT_PIN = 21;
//---------------------------------------------------------------------
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

const int SS_RX_PIN = 3;
const int SS_TX_PIN = 4;
//----------------------------------------------------------------
TaskHandle_t hled;
TaskHandle_t huart;
TaskHandle_t hcan;

CanHackerLineReader *lineReader = NULL;
CanHacker *canHacker = NULL;

SoftwareSerial softwareSerial(SS_RX_PIN, SS_TX_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  SPI.begin();
  softwareSerial.begin(115200);

  Stream *interfaceStream = &Serial;
  Stream *debugStream = &softwareSerial;


  canHacker = new CanHacker(interfaceStream, debugStream, SPI_CS_PIN);

  canHacker->setClock(MCP_8MHZ);    // For 8MHz crystal oscillator
  //canHacker->enableLoopback(); // uncomment this for loopback
  lineReader = new CanHackerLineReader(canHacker);

  pinMode(INT_PIN, INPUT);
  //--------------- create task----------------------------------
  xTaskCreatePinnedToCore(
    vLEDTask, "LEDTask" // A name just for humans
    ,
    1024 // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    &hled //handle
    ,
    0);
  //----------------------------------------
  xTaskCreatePinnedToCore(
    vCANTask, "CANTask"
    ,
    1024 // Stack size
    ,
    NULL, 1 // Priority
    ,
    &hcan
    ,
    1);

  Serial.println("Systom On!");

}
void loop() {
  /**
    CanHacker::ERROR error;

    if (digitalRead(INT_PIN) == LOW) {
      error = canHacker->processInterrupt();
      handleError(error);
    }

    error = lineReader->process();
    handleError(error);**/
}

static void vCANTask(void *pvParameters) {
  (void)pvParameters;

  Serial.println(F("CANTask at core:"));
  Serial.println(xPortGetCoreID());
  for (;;) {
  
    CanHacker::ERROR error;

    if (digitalRead(INT_PIN) == LOW) {
      error = canHacker->processInterrupt();
      handleError(error);
    }

    error = lineReader->process();
    handleError(error);
  }
  vTaskDelay(1);
}

static void vLEDTask(void *pvParameters) {
  (void)pvParameters;

  Serial.println(F("LEDTask at core:"));
  Serial.println(xPortGetCoreID());
  pinMode(LED_BUILTIN, OUTPUT);
  for (;;) {// A Task shall never return or exit.
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    vTaskDelay(200);
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    vTaskDelay(200);
  }
}

void handleError(const CanHacker::ERROR error) {

  switch (error) {
    case CanHacker::ERROR_OK:
    case CanHacker::ERROR_UNKNOWN_COMMAND:
    case CanHacker::ERROR_NOT_CONNECTED:
    case CanHacker::ERROR_MCP2515_ERRIF:
    case CanHacker::ERROR_INVALID_COMMAND:
      return;

    default:
      break;
  }

  softwareSerial.print("Failure (code ");
  softwareSerial.print((int)error);
  softwareSerial.println(")");

  digitalWrite(SPI_CS_PIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);

  while (1) {
    int c = (int)error;
    for (int i = 0; i < c; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }

    delay(2000);
  } ;
}