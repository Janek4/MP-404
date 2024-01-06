#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server(91, 103, 163, 85);
char user[] = "TEST_SUBJECT1";
char password[] = "#ovoANKhvH7";
char db[] = "DB_TEST";

char ssid[] = "KulisovaWiFi";
char pwd[] = "3215832158";

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
    float value1 = 100;
    float value2 = 150;
    float value3 = 3300;
    float value4 = 14;

    // Příprava SQL dotazu
    char query[256];
    sprintf(query, "INSERT INTO 'DATA' ('TEMP', 'SPEED', 'RPMS', 'VOLTAGE') VALUES (%f, %f, %f, %f)", value1, value2, value3, value4);

    // Vykonání SQL dotazu
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    previousMillis = currentMillis;  // Uložení aktuálního času
  }
}
