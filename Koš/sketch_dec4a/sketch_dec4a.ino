#include <can.h>
#include <mcp2515.h>

#include <CanHacker.h>
#include <CanHackerLineReader.h>
#include <lib.h>

#include <SPI.h>
#include <SoftwareSerial.h>

const int SPI_CS_PIN = 5;
const int INT_PIN = 21;

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

const int SS_RX_PIN = 3;
const int SS_TX_PIN = 4;

TaskHandle_t hled;
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

  xTaskCreatePinnedToCore(
    vLEDTask, "LEDTask",
    1024,
    NULL, 2,
    &hled,
    0);

  xTaskCreatePinnedToCore(
    vCANTask, "CANTask",
    1024,
    NULL, 1,
    &hcan,
    1);
}

void loop() {
  // Pokud používáte úlohy, nemusíte nic dělat zde
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

    vTaskDelay(1);
  }
}

static void vLEDTask(void *pvParameters) {
  (void)pvParameters;

  Serial.println(F("LEDTask at core:"));
  Serial.println(xPortGetCoreID());
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(200);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(200);
  }
}

void handleError(const CanHacker::ERROR error) {
  // Vaše implementace zůstává nezměněná
}
