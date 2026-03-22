#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "display/display.h"
#include "encoder/encoder.h"
#include "menu/menu.h"
#include "config/config.h"
#include "app/app.h"
#include "network/ethernet.h"

byte mac[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress serverIP(192, 168, 1, 1);
EthernetClient client;
uint16_t transID = 0;

void setup() {
  Serial.begin(115200);

  if (!inicjalizujEthernet()) {
    while (true) delay(1000);
  }

  Wire.begin(21, 22);  // I2C po Ethernecie
  inicjalizujDisplay();
  inicjalizujEnkoder();
  inicjalizujMenu();
  inicjalizujKonfigurator();
  pokazTekst("Modbus Tester", "v1.0");
  delay(1000);

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
      } else if (wynik == WynikMenu::WYSLIJ) {
        Serial.println("WYSLIJ kliknieto");
        wyslijPakietModbus(stan.master);
        delay(1000);
        uint8_t rozmiar = 0;
        const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
        pokazMenu(pozycje, rozmiar, stan.kursor);
      } else {
        uint8_t rozmiar = 0;
        const PozycjaMenu* pozycje = pobierzPozycjeMenu(rozmiar);
        pokazMenu(pozycje, rozmiar, stan.kursor);
      }
    }
  }
}
