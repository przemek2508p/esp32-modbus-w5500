#include "modbus_tcp.h"
#include <Ethernet.h>

extern EthernetClient client;
extern uint16_t transID;

void budujNaglowek(byte* req, uint8_t fc, uint16_t adres, uint16_t wartosc) {
  transID++;
  req[0] = highByte(transID); req[1] = lowByte(transID);
  req[2] = 0x00;              req[3] = 0x00;
  req[4] = 0x00;              req[5] = 0x06;
  req[6] = 0x01;
  req[7] = fc;
  req[8] = highByte(adres);   req[9] = lowByte(adres);
  req[10] = highByte(wartosc); req[11] = lowByte(wartosc);
}

void wyslijPakiet(byte* req, uint8_t len) {
  client.write(req, len);
  delay(100);
}