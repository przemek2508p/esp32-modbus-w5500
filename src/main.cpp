#include <Arduino.h>
#include <SPI.h>
#include "display/display.h"
#include "encoder/encoder.h"
#include "menu/menu.h"

void setup() {
  Serial.begin(115200);

  inicjalizujDisplay();
  inicjalizujEnkoder();
  inicjalizujMenu();
  pokazTekst("Modbus Tester", "v1.0");
  delay(1500);
}

void loop() {
  static int ostatniaPozycja = 0;
  int aktualna = pobierzPozycje();

  int delta = 0;
  if (aktualna > ostatniaPozycja) delta = 1;
  if (aktualna < ostatniaPozycja) delta = -1;
  ostatniaPozycja = aktualna;

  bool przycisk = pobierzPrzycisk();

  if (delta != 0 || przycisk) {
    aktualizujMenu(delta, przycisk);
    StanMenu& stan = pobierzStan();
    uint8_t rozmiar = 0;
    const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
    pokazMenu(pozycje, rozmiar, stan.kursor);
  }
}