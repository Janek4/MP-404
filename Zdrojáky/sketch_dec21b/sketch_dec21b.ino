#include <BluetoothSerial.h>
#include "ELMduino.h"
#include <TM1637Display.h>

const bool DEBUG        = true;
const int  TIMEOUT      = 1000;
const bool HALT_ON_FAIL = false;
BluetoothSerial SerialBT;
ELM327 ELMo;
#define ELM_PORT SerialBT
#define DEBUG_PORT Serial
// Piny pro první displej
#define CLK_PIN_1 14 // Připojeno k pinu CLK prvního displeje  zelený
#define DIO_PIN_1 27 // Připojeno k pinu DIO prvního displeje  modrý
// Piny pro druhý displej
#define CLK_PIN_2 33 // Připojeno k pinu CLK druhého displeje  zelený
#define DIO_PIN_2 32 // Připojeno k pinu DIO druhého displeje  modrý
TM1637Display display1(CLK_PIN_1, DIO_PIN_1);
TM1637Display display2(CLK_PIN_2, DIO_PIN_2);

typedef enum {ENG_RPM, SPEED, TEMPERATURE, VOLTAGE} obd_pid_states;
obd_pid_states obd_state = ENG_RPM;

float rpm = 0;
float kph = 0;
float temp = 0;
float volt = 0;

void setup() {
  display1.setBrightness(7); // Nastav jas prvního displeje na max
  display2.setBrightness(7); // Nastav jas modrého displeje 0 - 7


  Serial.begin(115200);
  ELM_PORT.begin(115200);
  //SerialBT.setPin("1234");
  ELM_PORT.begin("OBD II", true);
  if (!ELM_PORT.connect("OBD II")) {
      DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
      while (1);
  }

  if (!ELMo.begin(ELM_PORT, true, 2000)) {
      Serial.println("Couldn't connect to OBD scanner - Phase 2");
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
        display2.showNumberDec(rpm, true);
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
        display1.showNumberDec(kph, true);
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
        //display2.showNumberDec(volt, true);
        obd_state = ENG_RPM;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = ENG_RPM;
      }
      break;
    }
  }
  //display1.showNumberDec(kph, true);
  
}