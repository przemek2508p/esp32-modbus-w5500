#include "display.h"
#include <Wire.h>
#include <Arduino.h>

// 1602A — 16 kolumn, 2 wiersze, adres I2C 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool inicjalizujDisplay() {
  Wire.begin(21, 22);  // SDA=21, SCL=22
  lcd.init();
  lcd.backlight();
  Serial.println("[OK] LCD gotowy");
  return true;
}

void pokazTekst(const char* linia1, const char* linia2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linia1);
  if (linia2) {
    lcd.setCursor(0, 1);
    lcd.print(linia2);
  }
}

void pokazMenu(const PozycjaMenu* pozycje, uint8_t rozmiar, uint8_t kursor) {
  lcd.clear();
  // LCD 1602 ma tylko 2 wiersze — pokazuj aktualną i następną pozycję
  for (uint8_t i = 0; i < 2 && i < rozmiar; i++) {
    uint8_t idx = (kursor + i) % rozmiar;
    lcd.setCursor(0, i);
    if (i == 0) lcd.print("> ");
    else        lcd.print("  ");
    lcd.print(pozycje[idx].nazwa);
    if (!pozycje[idx].dostepna) lcd.print("..");
  }
}

void pokazEdycje(StanEdycji& edycja, KonfiguracjaMaster& master) {
  char buf[17];
  if (edycja.tryb == TrybEdycji::IP) {
    formatujIP(buf, master.ip, edycja.oktet);
    pokazTekst("IP serwera:", buf);
  } else if (edycja.tryb == TrybEdycji::PORT) {
    formatujWartosc(buf, master.port);
    pokazTekst("Port:", buf);
  } else if (edycja.tryb == TrybEdycji::SLAVE_ID) {
    formatujWartosc(buf, master.slaveID);
    pokazTekst("Slave ID:", buf);
  } else if (edycja.tryb == TrybEdycji::FC) {
    formatujWartosc(buf, master.fc);
    pokazTekst("Func Code:", buf);
  } else if (edycja.tryb == TrybEdycji::ADRES) {
    formatujWartosc(buf, master.adres);
    pokazTekst("Adres:", buf);
  } else if (edycja.tryb == TrybEdycji::WARTOSC) {
    formatujWartosc(buf, master.wartosc);
    pokazTekst("Wartosc:", buf);
  }
}

void wstrzymajDisplay() {
  // I2C — nic nie trzeba robić, nie koliduje z SPI
}

void wznowDisplay() {
  // I2C — nic nie trzeba robić
}