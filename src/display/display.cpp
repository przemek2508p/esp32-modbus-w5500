#include "display.h"
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <SPI.h>
#include "../menu/menu.h" 

#define OLED_CLK  14
#define OLED_MOSI 13
#define OLED_RES  26
#define OLED_DC   27
#define OLED_CS   15

Adafruit_SSD1306 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RES, OLED_CS);

bool inicjalizujDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("[BLAD] OLED nie znaleziony!");
    return false;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  Serial.println("[OK] OLED gotowy");
  return true;
}

void pokazTekst(const char* linia1, const char* linia2) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(linia1);
  if (linia2) display.println(linia2);
  display.display();
}

void pokazMenu(const PozycjaMenu* pozycje, uint8_t rozmiar, uint8_t kursor) {
  display.clearDisplay();

  for (uint8_t i = 0; i < rozmiar; i++) {
    // podświetl aktualną pozycję
    if (i == kursor) {
      display.fillRect(0, i * 10, 128, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK); // tekst czarny na białym tle
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(4, i * 10 + 1);
    display.print(pozycje[i].nazwa);

    // oznacz zablokowane pozycje
    if (!pozycje[i].dostepna) {
      display.print(" ...");
    }
  }

  display.display();
}