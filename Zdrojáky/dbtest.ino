#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server(91, 103, 163, 85);
char user[] = "TEST_SUBJECT1";
char password[] = "";
char db[] = "DB_TEST";

char ssid[] = "WiFi_SSID";
char pwd[] = "WiFi_PASSWORD";

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
    float value1 = random(0, 130);
    float value2 = random(0, 200);
    float value3 = random(798, 4500);
    float value4 = random(4, 14.9);

    char query[256];
    sprintf(query, "INSERT INTO DATA (value1, value2, value3, value4) VALUES (%f, %f, %f, %f)", value1, value2, value3, value4);

    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    previousMillis = currentMillis;
  }
}
