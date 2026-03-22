#include "modbus_tcp.h"
#include <Ethernet.h>

extern EthernetClient client;
extern uint16_t transID;

void budujNaglowek(byte* req, uint8_t fc, uint16_t adres, uint16_t wartosc, uint8_t slaveID) {
  transID++;
  req[0] = highByte(transID);  req[1] = lowByte(transID);
  req[2] = 0x00;               req[3] = 0x00;
  req[4] = 0x00;               req[5] = 0x06;
  req[6] = slaveID;
  req[7] = fc;
  req[8] = highByte(adres);    req[9] = lowByte(adres);
  req[10] = highByte(wartosc); req[11] = lowByte(wartosc);
}

void wyslijPakiet(byte* req, uint8_t len) {
  size_t wyslano = client.write(req, len);
  Serial.print("[TCP] write() wyslano: ");
  Serial.println(wyslano);
  Serial.print("[TCP] dane: ");
  for (int i = 0; i < len; i++) {
    if (req[i] < 0x10) Serial.print("0");
    Serial.print(req[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(100);
}