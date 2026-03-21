#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100); // Adres IP klienta Modbus
IPAddress serverIP(192, 168, 1, 1); // Adres IP serwera Modbus

EthernetClient client;
uint16_t transID = 0;

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

// FC05 - zapis pojedynczego Coil
void zapiszCoil(uint16_t adres, bool wartosc) {
  transID++;
  byte req[12] = {
    highByte(transID), lowByte(transID), // Transaction ID
    0x00, 0x00,                           // Protocol ID
    0x00, 0x06,                           // Length
    0x01,                                 // Slave ID
    0x05,                                 // FC05
    highByte(adres), lowByte(adres),      // Adres coila
    (byte)(wartosc ? 0xFF : 0x00), 0x00   // Wartość
  };
  client.write(req, 12);
  delay(100);
  Serial.print("[FC05] Zapis Coil[");
  Serial.print(adres);
  Serial.print("] = ");
  Serial.println(wartosc);
}

// FC01 - odczyt Coils
void odczytajCoil(uint16_t adres, uint16_t ilosc) {
  transID++;
  byte req[12] = {
    highByte(transID), lowByte(transID),
    0x00, 0x00,
    0x00, 0x06,
    0x01,
    0x01,                                  // FC01
    highByte(adres), lowByte(adres),
    highByte(ilosc), lowByte(ilosc)
  };
  client.write(req, 12);
  delay(100);

  if (client.available() >= 10) {
    byte resp[10];
    client.read(resp, 10);
    bool wartosc = resp[9] & 0x01;
    Serial.print("[FC01] Odczyt Coil[");
    Serial.print(adres);
    Serial.print("] = ");
    Serial.println(wartosc);
  }
}

void setup() {
  Serial.begin(115200);
  Ethernet.init(5);
  Ethernet.begin(mac, ip);
  delay(2000);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("[BLAD] W5500 nie znaleziony!");
    while (true) delay(1000);
  }
  Serial.print("[OK] IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  static uint32_t t = 0;
  static bool toggle = false;

  if (millis() - t >= 3000) {
    t = millis();
    if (!polacz()) return;

    odczytajCoil(0, 1);
    toggle = !toggle;
    zapiszCoil(0, toggle);
    Serial.println("---");
  }
}