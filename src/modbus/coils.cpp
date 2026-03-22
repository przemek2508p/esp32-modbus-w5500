#include "coils.h"
#include "modbus_tcp.h"
#include "../display/display.h"
#include <Ethernet.h>
#include <Arduino.h>

extern EthernetClient client;

bool zapiszCoil(uint16_t adres, uint16_t wartosc, uint8_t slaveID) {
  byte req[12];
  budujNaglowek(req, 0x05, adres, wartosc ? 0xFF00 : 0x0000, slaveID);
  wyslijPakiet(req, 12);

  Serial.print("[FC05] Zapis Coil[");
  Serial.print(adres);
  Serial.print("] = ");
  Serial.println(wartosc);

  delay(200);
  if (client.available() >= 12) {
    byte resp[12];
    client.read(resp, 12);
    pokazTekst("FC05 OK", "Coil zapisany");
    return true;
  }

  pokazTekst("FC05", "Brak odpowiedzi");
  return false;
}

bool odczytajCoil(uint16_t adres, uint16_t ilosc, uint8_t slaveID) {
  byte req[12];
  budujNaglowek(req, 0x01, adres, ilosc, slaveID);
  wyslijPakiet(req, 12);

  delay(200);
  if (client.available() >= 10) {
    byte resp[10];
    client.read(resp, 10);
    bool wartosc = resp[9] & 0x01;

    Serial.print("[FC01] Odczyt Coil[");
    Serial.print(adres);
    Serial.print("] = ");
    Serial.println(wartosc);

    char buf[16];
    sprintf(buf, "Coil[%d]=%d", adres, wartosc);
    pokazTekst("FC01 Odczyt:", buf);
    return true;
  }

  pokazTekst("FC01", "Brak odpowiedzi");
  Serial.println("[BLAD] FC01 brak odpowiedzi");
  return false;
}