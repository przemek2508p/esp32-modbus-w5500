# ESP32 + W5500 + LCD + Enkoder — Modbus Tester

Narzędzie do testowania Modbus TCP — tryb Master i Slave, konfiguracja przez enkoder i wyświetlacz LCD.

---

## Podłączenie

### W5500 → ESP32 (VSPI)

| W5500 | ESP32   |
|-------|---------|
| MOSI  | GPIO 23 |
| MISO  | GPIO 19 |
| SCLK  | GPIO 18 |
| SCS   | GPIO 5  |
| 3.3V  | 3.3V    |
| GND   | GND     |

### LCD 1602A I2C → ESP32

| LCD  | ESP32   | Opis |
|------|---------|------|
| GND  | GND     | Masa |
| VCC  | VIN (5V)| Zasilanie |
| SDA  | GPIO 21 | Dane I2C |
| SCL  | GPIO 22 | Zegar I2C |

> **Ważne:** LCD wymaga **5V** — podłącz do pinu `VIN`, nie `3.3V`.

> **Ważne:** Ethernet (W5500) musi być inicjalizowany przed LCD — `Wire.begin()` po `Ethernet.begin()`.

### Enkoder → ESP32

| Enkoder | ESP32   | Opis |
|---------|---------|------|
| CLK     | GPIO 32 | Obrót A |
| DT      | GPIO 33 | Obrót B |
| SW      | GPIO 25 | Przycisk |
| +       | 3.3V    | Zasilanie |
| GND     | GND     | Masa |

---

## Konfiguracja IP

**`ip`** — adres który dostaje ESP32, np. `192.168.1.100`

**`serverIP`** — adres PC z Modbus Slave. Przy połączeniu bezpośrednim kablem ustaw statyczne IP na karcie sieciowej PC, np. `192.168.1.1`.

> Oba urządzenia muszą być w tej samej podsieci.

> **Modbus Slave:** ustaw konkretne IP (`192.168.1.1`) zamiast `Any Address` — ESP32 nie nawiązuje połączenia gdy Slave nasłuchuje na `Any Address`.

---

## Typy rejestrów Modbus

| Typ | Adres | Dostęp | Co trzyma? | Przykład |
|-----|-------|--------|------------|---------|
| **Coils** | 0x 00001+ | Odczyt + Zapis | Bit (0/1) | LED, przekaźnik |
| Discrete Inputs | 1x 10001+ | Tylko odczyt | Bit (0/1) | Przycisk, czujnik |
| Input Registers | 3x 30001+ | Tylko odczyt | Liczba 16-bit | Temperatura, ADC |
| Holding Registers | 4x 40001+ | Odczyt + Zapis | Liczba 16-bit | Setpoint, parametry |

---

## Plan etapów

- [x] Etap 1 — Modbus TCP Client, obsługa Coils
- [x] Etap 2 — LCD hello world
- [x] Etap 3 — Enkoder, odczyt obrotów i przycisku
- [x] Etap 4 — Menu nawigacja LCD + enkoder
- [x] Etap 5 — Konfiguracja IP, Port, Slave ID przez menu
- [x] Etap 6 — Integracja konfiguracji z Modbusem
- [ ] Etap 7 — Tryb Slave

---

## Etap 2 — LCD

Moduł `src/display/` obsługuje wyświetlacz LCD 1602A przez I2C.

> Zamieniono OLED SSD1306 SPI na LCD 1602A I2C — SPI OLEDa kolidowało z magistralą W5500 podczas nawiązywania połączenia TCP.

```mermaid
flowchart TD
    A["inicjalizujDisplay()"]:::blue --> B["Wire.begin(21,22)<br/>lcd.init()<br/>lcd.backlight()"]:::teal
    B --> E["Serial: OK<br/>return true"]:::green

    F["pokazTekst(linia1, linia2)"]:::blue --> G["lcd.clear()"]:::teal
    G --> H["setCursor(0,0)<br/>print(linia1)"]:::teal
    H --> I{"linia2?"}:::purple
    I -- tak --> J["setCursor(0,1)<br/>print(linia2)"]:::teal
    I -- nie --> K["koniec"]:::gray
    J --> K

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```

---

## Etap 3 — Enkoder

Moduł `src/encoder/` obsługuje enkoder obrotowy z przyciskiem przez przerwania (interrupts).

> **Ważne:** Oba piny CLK i DT mają przerwania na `CHANGE` — algorytm sprawdza kombinację obu stanów co eliminuje skoki pozycji. Przycisk ma debouncing 300ms.

```mermaid
flowchart TD
    A["inicjalizujEnkoder()"]:::blue --> B["pinMode CLK/DT/SW<br/>INPUT_PULLUP"]:::teal
    B --> C["ostatniStan = CLK<<1 | DT"]:::teal
    C --> D["attachInterrupt CLK/DT → CHANGE<br/>attachInterrupt SW → FALLING"]:::teal
    D --> E["Serial: OK"]:::green

    F["ISR obslugaObrotu()"]:::blue --> G["odczyt CLK i DT<br/>stan = CLK<<1 | DT"]:::teal
    G --> H{"stan == ostatniStan?"}:::purple
    H -- tak --> I["return"]:::gray
    H -- nie --> J{"ostatniStan==01<br/>stan==00?"}:::purple
    J -- tak --> K["pozycja++"]:::green
    J -- nie --> L{"ostatniStan==10<br/>stan==00?"}:::purple
    L -- tak --> M["pozycja--"]:::red
    L -- nie --> N["ostatniStan = stan"]:::gray
    K --> N
    M --> N

    O["ISR obslugaPrzycisku()"]:::blue --> P{"teraz - ostatniCzas<br/>> 300ms?"}:::purple
    P -- nie --> Q["return"]:::gray
    P -- tak --> R["przyciskWcisniety = true<br/>ostatniCzas = teraz"]:::green

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```

---

## Etap 4 — Menu

Moduł `src/menu/` obsługuje nawigację — obrót góra/dół, przycisk = wejdź/zatwierdź.

Na start zaimplementowana tylko ścieżka **Master → Coils**. Slave widoczny w menu ale oznaczony jako `wkrótce`.

### Typy danych (`menu.h`)

| Typ | Opis |
|-----|------|
| `TypMenu` | Enum — identyfikator aktywnego podmenu |
| `TrybEdycji` | Enum — który parametr jest edytowany |
| `WynikMenu` | Enum — wynik aktualizacji menu (nawigacja / edycja / podmenu / wyślij) |
| `PozycjaMenu` | Jeden wiersz na LCD — nazwa, cel, dostępność, tryb edycji |
| `KonfiguracjaMaster` | Parametry: IP, Port, Slave ID, FC, adres, wartość |
| `StanMenu` | Aktualny stan — aktywne podmenu, kursor, konfiguracja |

### Struktura menu

```
[Główne menu]
├── Master
│   ├── Połączenie
│   │   ├── IP serwera
│   │   ├── Port
│   │   └── Slave ID
│   └── Pakiet
│       ├── Function Code
│       ├── Adres rejestru
│       ├── Wartość
│       └── Wyślij
└── Slave                        ← wkrótce
```

### Sterowanie

| Akcja | Efekt |
|-------|-------|
| Obrót w prawo | Następna pozycja |
| Obrót w lewo | Poprzednia pozycja |
| Przycisk | Wejdź głębiej / zatwierdź |

```mermaid
flowchart TD
    A["aktualizujMenu(delta, przycisk)"]:::blue --> B["aktualnePositcje()"]:::teal
    B --> C{"delta != 0?"}:::purple
    C -- tak --> D["kursor += delta<br/>ogranicz 0..rozmiar-1"]:::teal
    D --> E["return NAWIGACJA"]:::green
    C -- nie --> F{"przycisk?"}:::purple
    F -- nie --> G["return BRAK"]:::gray
    F -- tak --> H{"pozycja.dostepna?"}:::purple
    H -- nie --> G
    H -- tak --> I{"edycja != BRAK?"}:::purple
    I -- tak --> J["wejdzWEdycje()<br/>return WEJSCIE_EDYCJA"]:::amber
    I -- nie --> K{"nazwa == Wyslij?"}:::purple
    K -- tak --> L["return WYSLIJ"]:::amber
    K -- nie --> M["aktywne = cel<br/>kursor = 0<br/>return WEJSCIE_PODMENU"]:::green

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef amber fill:#FAEEDA,stroke:#854F0B,color:#633806
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```

---

## Etap 5 — Konfiguracja

Moduł `src/config/` obsługuje edycję wartości przez enkoder i formatowanie ich na LCD.

### Sterowanie podczas edycji

| Akcja | Efekt |
|-------|-------|
| Obrót | Zmień wartość (+1 / -1) |
| Przycisk | Zatwierdź i przejdź dalej (IP: następny oktet) |

---

## Etap 6 — Integracja Modbus

Moduł `src/app/` łączy konfigurację z menu z wysyłaniem pakietów Modbus TCP.

Po wybraniu "Wyślij" w menu — parametry z `KonfiguracjaMaster` trafiają do funkcji Modbus.

```mermaid
flowchart TD
    A["wyslijPakietModbus(cfg)"]:::blue --> B["serverIP = cfg.ip"]:::teal
    B --> C["polacz()"]:::teal
    C --> D{"connected?"}:::purple
    D -- nie --> E["pokazTekst BLAD<br/>return false"]:::red
    D -- tak --> F{"cfg.fc?"}:::purple
    F -- 0x01 --> G["odczytajCoil(adres, ilosc, slaveID)"]:::teal
    F -- 0x05 --> H["zapiszCoil(adres, wartosc, slaveID)"]:::teal
    G --> I["delay 200ms<br/>client.available >= 10?"]:::purple
    I -- tak --> J["odczyt resp[9]<br/>pokazTekst wynik"]:::green
    I -- nie --> K["pokazTekst brak odpowiedzi"]:::red
    H --> L["delay 200ms<br/>client.available >= 12?"]:::purple
    L -- tak --> M["pokazTekst FC05 OK"]:::green
    L -- nie --> K

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
```

---

## Etap 7 — Tryb Slave

> *do uzupełnienia*

### Znane problemy do rozwiązania

1. **Edycja IP na LCD 1602A** — wprowadzanie adresu IP przez enkoder nie działa poprawnie na nowym wyświetlaczu. Do zbadania i naprawy.

2. **Przyczyna problemu z SPI** — możliwe że OLED SPI nie był przyczyną braku połączenia. Problem mógł leżeć po stronie Modbus Slave który był skonfigurowany na `Any Address` zamiast konkretnego IP `192.168.1.1`. Do weryfikacji czy stary OLED SSD1306 działa poprawnie po ustawieniu stałego IP w Modbus Slave.