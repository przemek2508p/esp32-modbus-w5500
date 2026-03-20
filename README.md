# ESP32 + W5500 — Modbus TCP Client

---

## Podłączenie W5500 → ESP32

| W5500 | ESP32   |
|-------|---------|
| MOSI  | GPIO 23 |
| MISO  | GPIO 19 |
| SCLK  | GPIO 18 |
| SCS   | GPIO 5  |
| 3.3V  | 3.3V    |
| GND   | GND     |

## Typy rejestrów Modbus

| Typ | Adres | Dostęp | Co trzyma? | Przykład |
|-----|-------|--------|------------|---------|
| **Coils** | 0x 00001+ | Odczyt + Zapis | Bit (0/1) | LED, przekaźnik |
| Discrete Inputs | 1x 10001+ | Tylko odczyt | Bit (0/1) | Przycisk, czujnik |
| Input Registers | 3x 30001+ | Tylko odczyt | Liczba 16-bit | Temperatura, ADC |
| Holding Registers | 4x 40001+ | Odczyt + Zapis | Liczba 16-bit | Setpoint, parametry |

---

## Diagram działania kodu — Coils

```mermaid
flowchart TD
    A([Start]) --> B[Ethernet.init + Ethernet.begin]
    B --> C{W5500 OK?}
    C -- nie --> ERR([Błąd - sprawdź kable])
    C -- tak --> D[mb.client]
    D --> LOOP([Pętla loop])
    LOOP --> E{mb.isConnected?}
    E -- nie --> F[mb.connect port 502]
    F --> E
    E -- tak --> G[FC01 readCoil addr 0]
    G --> H[mb.task]
    H --> I[Serial.print odczyt]
    I --> J[toggle = !toggle]
    J --> K[FC05 writeCoil addr 0]
    K --> L[mb.task]
    L --> M[Serial.print zapis]
    M --> N[delay 3000ms]
    N --> LOOP
```