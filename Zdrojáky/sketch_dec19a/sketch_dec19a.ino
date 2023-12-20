#include "BluetoothSerial.h"
#include "ELMduino.h"

BluetoothSerial SerialBT;
#define ELM_PORT SerialBT
#define DEBUG_PORT Serial

ELM327 myELM327;

void setup() {
#if LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
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
    //time = millis();
    float rpm = myELM327.rpm();
    float kph = myELM327.kph();
    float temp = myELM327.engineCoolantTemp();
    float volts = myELM327.batteryVoltage();
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("Battery Voltage: ");
        Serial.println(volts);
        //delay();
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }

    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("RPMS: ");
        Serial.println(rpm);
        //delay();
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("SPEED in KM/H: ");
        Serial.println(kph);
        //delay();
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
        Serial.print("Coolant Temp.: ");
        Serial.println(temp);
        //delay();
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
        myELM327.printError();
    }
}
