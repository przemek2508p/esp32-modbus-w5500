#include "app.h"
#include "../modbus/coils.h"
#include "../network/ethernet.h"
#include "../display/display.h"
#include <Ethernet.h>
#include <Arduino.h>

extern IPAddress serverIP;

bool wyslijPakietModbus(KonfiguracjaMaster& cfg) {
  serverIP = IPAddress(cfg.ip[0], cfg.ip[1], cfg.ip[2], cfg.ip[3]);

  Serial.print("[APP] Laczenie z: ");
  Serial.print(serverIP);
  Serial.print(":");
  Serial.println(cfg.port);

  delay(100);
  if (!polacz()) {
    pokazTekst("BLAD", "Brak polaczenia");
    Serial.println("[APP] Brak polaczenia");
    return false;
  }

  Serial.println("[APP] Polaczono");

  if (cfg.fc == 0x01) return odczytajCoil(cfg.adres, cfg.wartosc, cfg.slaveID);
  if (cfg.fc == 0x05) return zapiszCoil(cfg.adres, cfg.wartosc, cfg.slaveID);

  pokazTekst("BLAD", "Nieznany FC");
  Serial.println("[APP] Nieznany FC");
  return false;
}