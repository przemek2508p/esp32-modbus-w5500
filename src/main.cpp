#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "network/ethernet.h"
#include "app/app.h"

byte mac[]       = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress serverIP(192, 168, 1, 1);
EthernetClient client;
uint16_t transID = 0;

void setup() {
  Serial.begin(115200);
  if (!inicjalizujEthernet()) {
    while (true) delay(1000);
  }
}

void loop() {
  static uint32_t t = 0;
  if (millis() - t >= 3000) {
    t = millis();
    if (!polacz()) return;
    wykonajCykl();
  }
}