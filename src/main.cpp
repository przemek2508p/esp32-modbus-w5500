#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ModbusIP_ESP8266.h>

byte mac[]          = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip        (192, 168, 1, 100);   // IP dla ESP32
IPAddress serverIP  (192, 168, 1, 10);    // ← IP Twojego PC (ipconfig)

ModbusIP mb;

void setup() {
  Serial.begin(115200);
  Ethernet.init(5);           // CS = GPIO 5
  Ethernet.begin(mac, ip);
  delay(1000);
  mb.client();
  Serial.println("Start");
}

void loop() {
  static uint32_t t = 0;
  static bool stan = false;

  if (!mb.isConnected(serverIP)) {
    mb.connect(serverIP, 502);
    delay(500);
  }

  if (millis() - t >= 3000) {
    t = millis();

    // Odczyt Coil nr 0
    bool odczyt = false;
    mb.readCoil(serverIP, 0, &odczyt, 1);
    mb.task();
    Serial.print("Odczyt Coil[0] = "); Serial.println(odczyt);

    // Zapis Coil nr 0 — przełącza co 3s
    stan = !stan;
    mb.writeCoil(serverIP, 0, stan);
    mb.task();
    Serial.print("Zapis  Coil[0] = "); Serial.println(stan);
    Serial.println("---");
  }

  mb.task();
  delay(10);
}