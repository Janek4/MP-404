#include <TM1637Display.h>

// Piny pro první displej
#define CLK_PIN_1 14 // Připojeno k pinu CLK prvního displeje
#define DIO_PIN_1 27 // Připojeno k pinu DIO prvního displeje
// Piny pro druhý displej
#define CLK_PIN_2 33 // Připojeno k pinu CLK druhého displeje
#define DIO_PIN_2 32 // Připojeno k pinu DIO druhého displeje

TM1637Display display1(CLK_PIN_1, DIO_PIN_1);
TM1637Display display2(CLK_PIN_2, DIO_PIN_2);

int counter1 = 990; // Počáteční hodnota pro první displej
int counter2 = 0;  // Počáteční hodnota pro druhý displej

void setup() {
  display1.setBrightness(7); // Nastav jas prvního displeje na max
  display2.setBrightness(7); // Nastav jas modrého displeje 0 - 7
}

void loop() {
  // Sníž číslo na prvním displeji
  display1.showNumberDec(counter1, true);
  counter1++;

  // Zvyšuj číslo na druhém displeji od nuly
  display2.showNumberDec(counter2, true);
  counter2++;

  delay(1); // Čekej 0.001 sekundy
}
