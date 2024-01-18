#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <BluetoothSerial.h>
#include "ELMduino.h"
#include <TM1637Display.h>

//const bool DEBUG        = true;
const int  TIMEOUT      = 2000;
const bool HALT_ON_FAIL = false;

IPAddress server(91, 103, 163, 85);
char user[] = "TEST_SUBJECT1";
char password[] = "#ovoANKhvH7";
char db[] = "DB_TEST";
char ssid[] = "2Girls1ESP";
char pwd[] = "87654321";
WiFiClient client;
MySQL_Connection conn(&client);

BluetoothSerial SerialBT;
ELM327 ELMo;

#define ELM_PORT SerialBT
#define DEBUG_PORT Serial
#define CLK_PIN_1 14 
#define DIO_PIN_1 27 
#define CLK_PIN_2 33
#define DIO_PIN_2 32
#define LED_BUILTIN 2

TM1637Display display1(CLK_PIN_1, DIO_PIN_1);
TM1637Display display2(CLK_PIN_2, DIO_PIN_2);

const unsigned long interval = 5000;
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;

typedef enum {ENG_RPM, SPEED, TEMPERATURE, VOLTAGE, FUEL_RATE} obd_pid_states;
obd_pid_states obd_state = ENG_RPM;

float rpm = 0;
float kph = 0;
float temp = 0;
float volt = 0;
float fuelRate = 0;

void connectToWiFi() {
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}
void setup() {
  display1.setBrightness(7);
  display2.setBrightness(2);
  
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  connectToWiFi();
  if (conn.connect(server, 3306, user, password, db)) {
    Serial.println("Connected to MySQL server");
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    Serial.println("Connection failed.");
    while (1);
  }

  ELM_PORT.begin(115200);
  SerialBT.setPin("1234");
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
        display1.showNumberDec(kph, false);
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
        obd_state = FUEL_RATE;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = FUEL_RATE;
      }
      break;
    }
    case FUEL_RATE: {
      fuelRate = ELMo.fuelRate();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("fuel R: ");
        Serial.println(fuelRate);
        display2.showNumberDec(fuelRate, false);
        obd_state = ENG_RPM;
      }
      else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = ENG_RPM;
      }
      break;
    }
  } 
  if (currentMillis - previousMillis >= interval) {
    char query[256];
    sprintf(query, "INSERT INTO DATA (TEMP, SPEED, RPMS, VOLTAGE) VALUES (%lf, %lf, %lf, %lf)", temp, kph, rpm, volt);

    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    previousMillis = currentMillis;
  }
}