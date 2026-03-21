#include "coils.h"
#include "modbus_tcp.h"
#include <Ethernet.h>

extern EthernetClient client;

void zapiszCoil(uint16_t adres, bool wartosc) {
  byte req[12];
  budujNaglowek(req, 0x05, adres, wartosc ? 0xFF00 : 0x0000);
  wyslijPakiet(req, 12);
  Serial.print("[FC05] Zapis Coil[");
  Serial.print(adres);
  Serial.print("] = ");
  Serial.println(wartosc);
}

void odczytajCoil(uint16_t adres, uint16_t ilosc) {
  byte req[12];
  budujNaglowek(req, 0x01, adres, ilosc);
  wyslijPakiet(req, 12);
  if (client.available() >= 10) {
    byte resp[10];
    client.read(resp, 10);
    Serial.print("[FC01] Odczyt Coil[");
    Serial.print(adres);
    Serial.print("] = ");
    Serial.println(resp[9] & 0x01);
  }
}