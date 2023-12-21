//#include <BluetoothSerial.h>
#include "ELMduino.h"
#define ELM_PORT Serial1

const bool DEBUG        = true;
const int  TIMEOUT      = 2000;
const bool HALT_ON_FAIL = false;
//BluetoothSerial SerialBT;
ELM327 ELMo;

typedef enum {ENG_RPM, SPEED, TEMPERATURE, VOLTAGE} obd_pid_states;
obd_pid_states obd_state = ENG_RPM;

float rpm = 0;
float kph = 0;
float temp = 0;
float volt = 0;

void setup() {
  Serial.begin(115200);
  //SerialBT.begin("OBD II");
  ELM_PORT.begin(115200);

  Serial.println("Attempting to connect to ELM327...");

  if (!ELMo.begin(ELM_PORT, DEBUG, TIMEOUT)) {
    Serial.println("Couldn't connect to OBD scanner");

    if (HALT_ON_FAIL)
      while (1);
  }

  Serial.println("Connected to ELM327");
}

void loop() {
  switch (obd_state) {
    case ENG_RPM: {
      rpm = ELMo.rpm();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("rpm: ");
        Serial.println(rpm);
        obd_state = SPEED;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = SPEED;
      }
      break;
    }
    
    case SPEED: {
      kph = ELMo.kph();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("kph: ");
        Serial.println(kph);
        obd_state = TEMPERATURE;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = TEMPERATURE;
      }
      break;
    }

    case TEMPERATURE: {
      temp = ELMo.engineCoolantTemp();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("temp: ");
        Serial.println(temp);
        obd_state = VOLTAGE;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = VOLTAGE;
      }
      break;
    }

    case VOLTAGE: {
      volt = ELMo.batteryVoltage();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("volt: ");
        Serial.println(volt);
        obd_state = ENG_RPM;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = ENG_RPM;
      }
      break;
    }
  }
}
