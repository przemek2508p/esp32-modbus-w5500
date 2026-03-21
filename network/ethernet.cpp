#include "ethernet.h"
#include <Arduino.h>
#include <SPI.h>

extern byte mac[];
extern IPAddress ip;
extern IPAddress serverIP;
extern EthernetClient client;

bool inicjalizujEthernet() {
  Ethernet.init(5);
  Ethernet.begin(mac, ip);
  delay(2000);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("[BLAD] W5500 nie znaleziony!");
    return false;
  }
  Serial.print("[OK] IP: ");
  Serial.println(Ethernet.localIP());
  return true;
}

bool polacz() {
  if (client.connected()) return true;
  client.stop();
  delay(100);
  if (client.connect(serverIP, 502)) {
    Serial.println("[OK] Polaczono");
    return true;
  }
  Serial.println("[BLAD] Brak polaczenia");
  return false;
}