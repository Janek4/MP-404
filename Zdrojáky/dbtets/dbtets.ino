#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server(91, 103, 163, 85);
char user[] = "TEST_SUBJECT1";
char password[] = "#ovoANKhvH7";
char db[] = "DB_TEST";

char ssid[] = "2Girls1ESP";
char pwd[] = "87654321";

WiFiClient client;
MySQL_Connection conn(&client);

const unsigned long interval = 2000;
unsigned long previousMillis = 0;

void connectToWiFi() {
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(115200);

  connectToWiFi();
  uint8_t mac[6];
  
  WiFi.macAddress(mac);
  Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (conn.connect(server, 3306, user, password, db)) {
    Serial.println("Connected to MySQL server");
  } else {
    Serial.println("Connection failed.");
    while (1);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Získání hodnot nebo senzorů
    float value1 = random(0, 130);  // Náhodné číslo od 0 do 100
    float value2 = random(0, 200); // Náhodné číslo od 50 do 200
    float value3 = random(798, 4500); // Náhodné číslo od 3000 do 4000
    float value4 = random(4, 14.9); // Náhodné číslo od 10 do 20
    float value5 = random(0, 15); // Náhodné číslo od 10 do 20

    // Příprava SQL dotazu
    char query[256];
    sprintf(query, "INSERT INTO DATA (TEMP, SPEED, RPMS, VOLTAGE, FUEL_RATE) VALUES (%lf, %lf, %lf, %lf, %lf)", value1, value2, value3, value4, value5);

    // Vykonání SQL dotazu
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    previousMillis = currentMillis;  // Uložení aktuálního času
  }
}
