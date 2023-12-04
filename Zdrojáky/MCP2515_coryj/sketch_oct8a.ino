#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
MCP_CAN CAN0(5);  // nastavit CS k pinu 5

void setup() {
  Serial.begin(115200);
  pinMode(21, INPUT);
  if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
     Serial.print("can init ok!!\r\n");
  } else {
     Serial.print("Can init fail!!\r\n");
  }
  Serial.println("MCP2515 Library Receive Example...");
}
void loop() {
  if (!digitalRead(21)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++) {
      if (rxBuf[i] < 0x10) {
        Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
