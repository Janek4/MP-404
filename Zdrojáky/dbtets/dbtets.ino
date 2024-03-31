#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server(91, 103, 163, 85);
char user[] = "ESP";
char password[] = "";
char db[] = "MP_404";

char ssid[] = "2Girls1ESP";
char pwd[] = "87654321";

WiFiClient client;
MySQL_Connection conn(&client);
#define LED_BUILTIN 2

const unsigned long interval = 25000;
unsigned long previousMillis = 0;
unsigned long previousDBCheckMillis = 0;
unsigned long previousDBConnectMillis = 0;
bool dbConnected = false;
int dbkph2;
void WiFiconn() {
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting to WiFi...");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void DBconn() {
  if (!dbConnected || (millis() - previousDBConnectMillis > 100)) {
    if (conn.connect(server, 3306, user, password, db)) {
      Serial.println("Connected to MySQL server");
      dbConnected = true;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      Serial.println("Connection to MySQL server failed.");
      dbConnected = false;
    }
    previousDBConnectMillis = millis();
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFiconn();
  DBconn();

  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousDBCheckMillis >= 30000) {
    Serial.println(dbConnected ? "Connected to database" : "Not connected to database");
    previousDBCheckMillis = currentMillis;
  }

  if (!dbConnected) {
    digitalWrite(LED_BUILTIN, LOW);
    DBconn();
  }

    float dbtemp = random(80, 105);  // Náhodné číslo od 0 do 100
    float dbkph = random(90, 120); // Náhodné číslo od 50 do 200
    float dbrpm = random(2200, 2500); // Náhodné číslo od 3000 do 4000
    float dbvolt = random(10, 14.9); // Náhodné číslo od 10 do 20
    //float value5 = random(0, 15); // Náhodné číslo od 10 do 20

     dbkph2 = int(dbkph * 1.123); // Převod na celé číslo

  // Pokud je připojení k databázi a uplynulo interval, vložíme data do databáze
  if (dbConnected && currentMillis - previousMillis >= interval && dbrpm != 0 && dbvolt != 0) {
    char query[128];
    sprintf(query, "INSERT INTO DATA (TEMP, SPEED, SPEED2, RPMS, VOLT) VALUES (%lf, %lf, %d, %lf, %lf)", dbtemp, dbkph, dbkph2, dbrpm, dbvolt);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;
    previousMillis = currentMillis;
  }
}
