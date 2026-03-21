# ESP32 + W5500 - Modbus TCP Client

## Podłączenie W5500 → ESP32

| W5500 | ESP32   |
|-------|---------|
| MOSI  | GPIO 23 |
| MISO  | GPIO 19 |
| SCLK  | GPIO 18 |
| SCS   | GPIO 5  |
| 3.3V  | 3.3V    |
| GND   | GND     |

---

## Konfiguracja IP

**`ip`** — adres który dostaje ESP32, np. `192.168.1.100`

**`serverIP`** — adres PC z Modbus Slave. Przy połączeniu bezpośrednim kablem ustaw statyczne IP na karcie sieciowej PC, np. `192.168.1.1`.

> Oba urządzenia muszą być w tej samej podsieci.

---

## Typy rejestrów Modbus

| Typ | Adres | Dostęp | Co trzyma? | Przykład |
|-----|-------|--------|------------|---------|
| **Coils** | 0x 00001+ | Odczyt + Zapis | Bit (0/1) | LED, przekaźnik |
| Discrete Inputs | 1x 10001+ | Tylko odczyt | Bit (0/1) | Przycisk, czujnik |
| Input Registers | 3x 30001+ | Tylko odczyt | Liczba 16-bit | Temperatura, ADC |
| Holding Registers | 4x 40001+ | Odczyt + Zapis | Liczba 16-bit | Setpoint, parametry |

Uczymy się po jednym typie — zaczynamy od **Coils**.

---

## Diagram działania kodu — Coils

```mermaid
flowchart TD
    START([setup]) --> INIT

    INIT["Ethernet.init(5)<br/>Ethernet.begin(mac, ip)"]:::blue --> W5500

    W5500{W5500 OK?}:::purple
    W5500 -- nie --> ERR["BŁĄD<br/>while(true)"]:::red
    W5500 -- tak --> SERIAL

    SERIAL["Serial: OK IP<br/>Ethernet.localIP()"]:::green --> LOOP

    LOOP([loop]):::gray --> TIMER

    TIMER{"millis - t<br/>>= 3000ms?"}:::purple
    TIMER -- nie --> CZEKA["czeka"]:::gray
    CZEKA --> LOOP
    TIMER -- tak --> POLACZ

    POLACZ["polacz()<br/>client.connect(serverIP, 502)"]:::purple --> CONN

    CONN{"connected?"}:::purple
    CONN -- nie --> RET["return<br/>brak poł."]:::red
    CONN -- tak --> READ

    READ["odczytajCoil(0, 1)<br/>FC01 → wyślij pakiet → Serial print"]:::teal --> TOGGLE

    TOGGLE["toggle = !toggle<br/>zmień stan 0→1 lub 1→0"]:::amber --> WRITE

    WRITE["zapiszCoil(0, toggle)<br/>FC05 → wyślij pakiet → Serial print"]:::teal --> RESET

    RESET["t = millis()<br/>reset timera"]:::gray --> LOOP

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef amber fill:#FAEEDA,stroke:#854F0B,color:#633806
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```