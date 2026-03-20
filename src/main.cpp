#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ModbusMaster.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 100);
IPAddress serverIP(192, 168, 0, 1);  // ← IP Twojego PC

EthernetClient client;
ModbusMaster node;

void setup() {
  Serial.begin(115200);
  Ethernet.init(5);
  Ethernet.begin(mac, ip);
  delay(1000);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("[BLAD] W5500 nie znaleziony!");
    while (true) delay(1000);
  }

  Serial.print("[OK] IP: ");
  Serial.println(Ethernet.localIP());

  // Połącz TCP z Modbus Poll
  if (client.connect(serverIP, 502)) {
    Serial.println("[OK] Polaczono z Modbus Poll");
    node.begin(1, client);  // Slave ID = 1
  } else {
    Serial.println("[BLAD] Brak polaczenia z serwerem");
  }
}

void loop() {
  static uint32_t t = 0;

  if (millis() - t >= 3000) {
    t = millis();

    // FC01 - odczyt Coil nr 0
    uint8_t result = node.readCoils(0, 1);

    if (result == node.ku8MBSuccess) {
      uint16_t val = node.getResponseBuffer(0);
      Serial.print("[FC01] Coil[0] = ");
      Serial.println(val);
    } else {
      Serial.print("[BLAD] FC01 kod: 0x");
      Serial.println(result, HEX);
    }

    delay(200);

    // FC05 - zapis Coil nr 0
    static bool toggle = false;
    toggle = !toggle;
    result = node.writeSingleCoil(0, toggle);

    if (result == node.ku8MBSuccess) {
      Serial.print("[FC05] Zapis Coil[0] = ");
      Serial.println(toggle);
    } else {
      Serial.print("[BLAD] FC05 kod: 0x");
      Serial.println(result, HEX);
    }

    Serial.println("---");
  }
}