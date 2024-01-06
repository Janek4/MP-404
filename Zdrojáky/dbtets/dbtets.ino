#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server(91, 103, 163, 85);
char user[] = "TEST_SUBJECT1";
char password[] = "";
char db[] = "DB_TEST";

const char *ssid = "2 Girls 1 Hotspot";
const char *password = "87654321";

WiFiClient client;
MySQL_Connection conn(&client);

const unsigned long interval = 5000;
unsigned long previousMillis = 0;

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(115200);

  connectToWiFi();

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
    float value1 = 1.23;
    float value2 = 4.56;
    float value3 = 7.89;
    float value4 = 0.12;

    // Příprava SQL dotazu
    char query[128];
    sprintf(query, "INSERT INTO DATA (column1, column2, column3, column4) VALUES (%f, %f, %f, %f)", value1, value2, value3, value4);

    // Vykonání SQL dotazu
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    previousMillis = currentMillis;  // Uložení aktuálního času
  }
}
