#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <BluetoothSerial.h>
#include "ELMduino.h"
#include <TM1637Display.h>

const int TIMEOUT = 2000;
const bool HALT_ON_FAIL = false;
IPAddress server(91, 103, 163, 85);
char user[] = "V40_ESP";
char password[] = "";
char db[] = "MP_404_V40";
char ssid[] = "2Girls1ESP";
char pwd[] = "87654321";

WiFiClient client;
MySQL_Connection conn(&client);
BluetoothSerial SerialBT;
ELM327 ELMo;

#define ELM_PORT SerialBT
#define DEBUG_PORT Serial
#define CLK_PIN_1 26
#define DIO_PIN_1 25 
#define CLK_PIN_2 33
#define DIO_PIN_2 32 //23 //34 //32
#define LED_BUILTIN 2
#define POT 35 
#define SWITCH1 34 //27
#define SWITCH2 39 //14

TM1637Display display1(CLK_PIN_1, DIO_PIN_1);
TM1637Display display2(CLK_PIN_2, DIO_PIN_2);
const unsigned long interval = 5000;
unsigned long previousMillis = 0;
unsigned long previousDBCheckMillis = 0;
unsigned long previousDBConnectMillis = 0;
bool dbConnected = false;
int numDBConnectFails = 0;
const int DBconnF = 3;
const float odchylka = 1.123;

typedef enum {ENG_RPM, SPEED, TEMPERATURE, VOLTAGE, FUEL} obd_pid_states;
//typedef enum {ENG_RPM, SPEED, TEMPERATURE, VOLTAGE} obd_pid_states;

obd_pid_states obd_state = ENG_RPM;
float rpm = 0;
float kph = 0;
float temp = 0;
float volt = 0;
float fuel = 0;
float dbrpm;
float dbkph;
float dbtemp;
float dbvolt;
float dbfuel;
int dbkph2;
int prevdbrpmcounter = 0;
float prevdbrpm;
float prevodnicek;

void WiFiconn() {
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting to WiFi...");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 7000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    digitalWrite(LED_BUILTIN, HIGH);
    
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    Serial.println("\nFailed to connect to WiFi");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void DBconn() {
  if (!dbConnected || (millis() - previousDBConnectMillis > 5000)) {
    if (conn.connect(server, 3306, user, password, db)) {
      Serial.println("Connected to MariaDB server");
      dbConnected = true;
      numDBConnectFails = 0;
    } else {
      dbConnected = false;
      numDBConnectFails++;
    }
    previousDBConnectMillis = millis();
  }
//funguje, když se tomu chce
  if (numDBConnectFails >= DBconnF) {
    Serial.println("max number of failed DB connection attempts. Rebooting..."); //soft reset
    delay(2000);
    ESP.restart();
  }
}

void BTconn() {
  SerialBT.begin("OBD II", true);
    if (!ELM_PORT.connect("OBD II")) {
      DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
      while (1);
    }
    if (!ELMo.begin(ELM_PORT, true, 4000)) {
      Serial.println("Couldn't connect to OBD scanner - Phase 2");
      while (1);
    }
    Serial.println("Connected to ELM327");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);

  
  WiFiconn();
  DBconn();
  BTconn();

  /*display1.setBrightness(4);
  display2.setBrightness(4);*/
  
}


void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillisDB = millis();
  if (currentMillis - previousDBCheckMillis >= 10000) {
    /*Serial.println(dbConnected ? "Connected to database" : "Not connected to database");*/
    previousDBCheckMillis = currentMillisDB;
  }

  int switch1s = digitalRead(SWITCH1);
  int switch2s = digitalRead(SWITCH2);

  
  int potValue = analogRead(POT);
  int bright = map(potValue, 0, 4095, 0, 7);
  Serial.print("brightness: ");
  Serial.println(bright);
  display1.setBrightness(bright);
  display2.setBrightness(bright);

  if (!dbConnected) {
    DBconn();
  }
  //čtení dat z OBD
  switch (obd_state) {
    case ENG_RPM: {
      rpm = ELMo.rpm();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("rpm: ");
        Serial.println(rpm);
        dbrpm = rpm;
        if (switch1s == LOW) {
          //switch2s = false;
          display2.showNumberDec(dbrpm, false);
        }
        obd_state = SPEED;
      } else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
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
        dbkph = kph;
        if (switch1s == LOW) {
          //switch1s = false;
          display1.showNumberDec(dbkph, false);
        } 
        obd_state = TEMPERATURE;
      } else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
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
        dbtemp = temp;
        if (switch1s == HIGH) {
          //switch1s = true;
          display1.showNumberDecEx(dbtemp, false);
        }
        obd_state = VOLTAGE;
      } else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
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
        dbvolt = volt;
        if (switch1s == HIGH) {
          //switch1s = true;
          display2.showNumberDecEx(dbvolt, false);
        }
        obd_state = FUEL;
        //obd_state = ENG_RPM;
      } else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = FUEL;
        //obd_state = ENG_RPM;
      }
      break;
    }

    case FUEL: {
      fuel = ELMo.fuelRate();
      if (ELMo.nb_rx_state == ELM_SUCCESS) {
        Serial.print("fuel: ");
        Serial.println(fuel);
        dbfuel = fuel;
        obd_state = ENG_RPM;
      } else if (ELMo.nb_rx_state != ELM_GETTING_MSG) {
        ELMo.printError();
        obd_state = ENG_RPM;
      }
      break;
    }
  }
  if (dbkph < 30) {
    dbkph2 = dbkph;
  }
  if (dbkph >= 30 && dbkph <= 100) {
    dbkph2 = dbkph + 5;
  }
  if (dbkph >= 101 && dbkph <= 150) {
    dbkph2 = dbkph + 10;
  }
  if (dbkph >= 151) {
    dbkph2 = dbkph + 5;
  }

  /*prevodnicek = dbkph * odchylka;
  dbkph2 = (int)prevodnicek;*/

  if (dbConnected && currentMillis - previousMillis >= interval && /*dbrpm >= 600 && */dbvolt != 0/* && dbvolt <= 20 && dbspeed <= 300*/) {
    if (dbrpm != prevdbrpm) {
      if (prevdbrpm == dbrpm) {
        prevdbrpmcounter++;
      } else {
        prevdbrpmcounter = 0;
      }
      prevdbrpm = dbrpm;
    }

    //nefunguje
    if (prevdbrpmcounter >= 15 || SerialBT.hasClient() == 0/* && dbrpm != 0*/) {
      dbrpm = 0;
      display1.clear();
      display2.clear();
      delay(5000);
      ESP.restart();
    }

    char query[128];
    //sprintf(query, "INSERT INTO DATA (TEMP, SPEED, SPEED2, RPMS, VOLT) VALUES (%lf, %lf, %d, %lf, %lf)", dbtemp, dbkph, dbkph2, dbrpm, dbvolt);
    sprintf(query, "INSERT INTO DATA (TEMP, SPEED, SPEED2, RPMS, VOLT, FUEL) VALUES (%lf, %lf, %d, %lf, %lf, %lf)", dbtemp, dbkph, dbkph2, dbrpm, dbvolt, dbfuel);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;
    previousMillis = currentMillis;

      
    }
}
