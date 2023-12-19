#include "BluetoothSerial.h"
#include "ELMduino.h"
#include <TM1637Display.h>
BluetoothSerial SerialBT;
#define ELM_PORT SerialBT
#define DEBUG_PORT Serial
#define LED_BUILTIN 2
// Piny pro první displej
#define CLK_PIN_1 14 // Připojeno k pinu CLK prvního displeje  zelený
#define DIO_PIN_1 27 // Připojeno k pinu DIO prvního displeje  modrý
// Piny pro druhý displej
#define CLK_PIN_2 33 // Připojeno k pinu CLK druhého displeje  zelený
#define DIO_PIN_2 32 // Připojeno k pinu DIO druhého displeje  modrý

TM1637Display display1(CLK_PIN_1, DIO_PIN_1);
TM1637Display display2(CLK_PIN_2, DIO_PIN_2);
ELM327 myELM327;

void setup() {
  display1.setBrightness(7); // Nastav jas prvního displeje na max
  display2.setBrightness(7); // Nastav jas modrého displeje 0 - 7
#if LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
    DEBUG_PORT.begin(115200);
    // SerialBT.setPin("1234");
    ELM_PORT.begin("OBD II", true);

    if (!ELM_PORT.connect("OBD II")) {
        DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
        while (1);
    }

    if (!myELM327.begin(ELM_PORT, true, 2000)) {
        Serial.println("Couldn't connect to OBD scanner - Phase 2");
        while (1);
    }

    Serial.println("Connected to ELM327");
}

void loop() {
    int time = millis();
    float rpm = myELM327.rpm();
    //float mph = myELM327.mph();
    float temp = myELM327.engineCoolantTemp();
    float volt = myELM327.batteryVoltage();
    //float kph = mph * 0.6213711922;
    int32_t kph myELM327.kph();

    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        // Print Battery Voltage
        Serial.print("Battery Voltage: ");
        Serial.println(volt);
        delay(100);

        // Print RPMS
        Serial.print("RPMS: ");
        Serial.println(rpm);
        display2.showNumberDec(rpm, true);
        delay(100);

        // Print Speed in KM/H
        Serial.print("SPEED in KM/H: ");
        Serial.println(kph);
        display1.showNumberDec(kph, true);
        delay(100);

        // Print Coolant Temperature
        Serial.print("Coolant Temp.: ");
        Serial.println(temp);
        delay(100);
    } else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        // If there is an error, print the error message
        myELM327.printError();
    }

    // Additional code can be added here if needed
}



    /*int time = millis();
    float rpm = myELM327.rpm();
    float mph = myELM327.mph();
    float temp = myELM327.engineCoolantTemp();
    float volt = myELM327.batteryVoltage();
    float kph = mph * 0.6213711922;
    if (time > 5000) {
      if (myELM327.nb_rx_state == ELM_SUCCESS) {
          Serial.print("Battery Voltage: ");
          Serial.println(volt);
          Serial.print("SPEED in KM/H: ");
          Serial.println(kph);
          Serial.print("RPMS: ");
          Serial.println(rpm);
          Serial.print("Coolant Temp.: ");
          Serial.println(temp);

          display1.showNumberDec(kph, true);
          display2.showNumberDec(rpm, true);
          //delay();
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
          myELM327.printError();
      }
    }
    */
    /*if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("Battery Voltage: ");
        Serial.println(volt);
        delay(100);
    }

    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("RPMS: ");
        Serial.println(rpm);
        
        display2.showNumberDec(rpm, true);
        delay(100);
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("SPEED in KM/H: ");
        Serial.println(kph);

        display1.showNumberDec(kph, true);
        delay(100);
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("Coolant Temp.: ");
        Serial.println(temp);
        delay(100);
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }*/
    
}