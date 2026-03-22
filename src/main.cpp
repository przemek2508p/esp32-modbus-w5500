#include <Arduino.h>
#include <SPI.h>
#include "display/display.h"
#include "encoder/encoder.h"
#include "menu/menu.h"
#include "config/config.h"

void setup() {
  Serial.begin(115200);

  inicjalizujDisplay();
  inicjalizujEnkoder();
  inicjalizujMenu();
  inicjalizujKonfigurator();
  pokazTekst("Modbus Tester", "v1.0");
  delay(1500);

  // pokaż menu od razu po starcie
  StanMenu& stan = pobierzStan();
  uint8_t rozmiar = 0;
  const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
  pokazMenu(pozycje, rozmiar, stan.kursor);
}

void loop() {
  static int ostatniaPozycja = 0;
  int aktualna = pobierzPozycje();

  int delta = 0;
  if (aktualna > ostatniaPozycja) delta = -1;
  if (aktualna < ostatniaPozycja) delta = 1;
  ostatniaPozycja = aktualna;

  bool przycisk = pobierzPrzycisk();

  if (delta != 0 || przycisk) {
    StanMenu&   stan   = pobierzStan();
    StanEdycji& edycja = pobierzStanEdycji();

    if (edycja.aktywna) {
      bool zakonczona = aktualizujEdycje(delta, przycisk, stan.master);
      if (zakonczona) {
        uint8_t rozmiar = 0;
        const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
        pokazMenu(pozycje, rozmiar, stan.kursor);
      } else {
        pokazEdycje(edycja, stan.master);
      }
    } else {
      WynikMenu wynik = aktualizujMenu(delta, przycisk);
      if (wynik == WynikMenu::WEJSCIE_EDYCJA) {
        pokazEdycje(edycja, stan.master);
      } else {
        uint8_t rozmiar = 0;
        const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
        pokazMenu(pozycje, rozmiar, stan.kursor);
      }
    }
  }
}