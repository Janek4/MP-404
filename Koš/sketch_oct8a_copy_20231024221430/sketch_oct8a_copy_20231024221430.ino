#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 // I2C adresa displeje, může být odlišná v závislosti na vašem displeji
#define SDA_PIN 26    // SDA pin, můžete změnit podle potřeby
#define SCL_PIN 27    // SCL pin, můžete změnit podle potřeby


long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
MCP_CAN CAN0(5);  // nastavit CS k pinu 5
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); //LiquidCrystal_I2C lcd(0x27, 16, 2);   I2C adresa 0x27, 16 sloupců a 2 řádky

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  lcd.backlight(); 
  // init can bus: baudrate = 500k
  if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("can init ok!!");
  } else {
    Serial.println("Can init fail!!");
    while (1);
    lcd.setCursor(0, 0);
    lcd.print("Hello, ESP32!");
  }
  
  pinMode(21, INPUT); // nastavení pinu 21 pro /INT vstup
  Serial.println("MCP2515 Library Receive Example...");

 /* lcd.init();                      // Inicializace LCD displeje
  lcd.backlight();                 // Zapnutí podsvícení*/
}

void loop() {
  if (!digitalRead(21)) { // Jestli pin 21 je low, čtení informací z bufferu
  
    CAN0.readMsgBuf(&rxId, &len, rxBuf); // čtení informací: rxId = ID, len = délka dat, rxBuf = data bytes
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++) { // Tisk každého bytu dat
      if (rxBuf[i] < 0x10) { // Jestli je byte dat míň než 0x10, přidat nulu před
        Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Převedení prvního bytu dat na desítkovou soustavu
    int decimalValue = rxBuf[0];
    
    // Zobrazení hodnoty na LCD displeji
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Received ID: ");
    lcd.print(rxId, HEX);
    lcd.setCursor(0, 1);
    lcd.print("Decimal Value: ");
    lcd.print(decimalValue);
  }
  else {
    // Pokud nebyla přijata žádná zpráva, zobrazíme statický text na displeji
    Serial.println("none");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for CAN");
    lcd.setCursor(0, 1);
    lcd.print("messages...");
  }
}
