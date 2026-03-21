#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "network/ethernet.h"
#include "app/app.h"
#include "display/display.h"

byte mac[]       = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress serverIP(192, 168, 1, 1);
EthernetClient client;
uint16_t transID = 0;

void setup() {
  Serial.begin(115200);

  inicjalizujDisplay();        // ← najpierw OLED
  pokazTekst("Modbus Tester", "v1.0");

  if (!inicjalizujEthernet()) { // ← potem Ethernet
    pokazTekst("BLAD", "W5500!");
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