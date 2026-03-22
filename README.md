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

## Struktura projektu

```
src/
├── main.cpp
├── network/
│   ├── ethernet.h
│   └── ethernet.cpp
├── modbus/
│   ├── modbus_tcp.h
│   ├── modbus_tcp.cpp
│   ├── coils.h
│   └── coils.cpp
└── app/
    ├── app.h
    └── app.cpp
```

---

## Diagram — `network/ethernet`

Inicjalizacja W5500 i zarządzanie połączeniem TCP.

```mermaid
flowchart TD
    A["inicjalizujEthernet()"]:::blue --> B["Ethernet.init(5)<br/>Ethernet.begin(mac, ip)"]:::blue
    B --> C{"W5500 OK?"}:::purple
    C -- nie --> D["return false"]:::red
    C -- tak --> E["Serial: OK IP<br/>return true"]:::green

    F["polacz()"]:::blue --> G{"client.connected?"}:::purple
    G -- tak --> H["return true"]:::green
    G -- nie --> I["client.stop()<br/>client.connect(serverIP, 502)"]:::blue
    I --> J{"połączono?"}:::purple
    J -- tak --> K["Serial: OK<br/>return true"]:::green
    J -- nie --> L["Serial: BŁĄD<br/>return false"]:::red

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
```

---

## Diagram — `modbus/modbus_tcp`

Budowanie nagłówka MBAP i wysyłanie pakietu TCP.

```mermaid
flowchart TD
    A["budujNaglowek(req, fc, adres, wartosc)"]:::blue
    A --> B["transID++"]:::gray
    B --> C["req[0..1] = transID"]:::gray
    C --> D["req[2..3] = 0x0000<br/>Protocol ID"]:::gray
    D --> E["req[4..5] = 0x0006<br/>Length"]:::gray
    E --> F["req[6] = 0x01<br/>Slave ID"]:::gray
    F --> G["req[7] = fc<br/>Function Code"]:::gray
    G --> H["req[8..9] = adres"]:::gray
    H --> I["req[10..11] = wartosc"]:::gray

    J["wyslijPakiet(req, len)"]:::blue --> K["client.write(req, len)"]:::teal
    K --> L["delay(100)"]:::gray

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```

---

## Diagram — `modbus/coils`

Odczyt (FC01) i zapis (FC05) pojedynczego Coil.

```mermaid
flowchart TD
    A["odczytajCoil(adres, ilosc)"]:::blue
    A --> B["budujNaglowek(req, 0x01, adres, ilosc)"]:::teal
    B --> C["wyslijPakiet(req, 12)"]:::teal
    C --> D{"client.available<br/>≥ 10 bajtów?"}:::purple
    D -- nie --> E["brak odpowiedzi"]:::red
    D -- tak --> F["client.read(resp, 10)"]:::teal
    F --> G["wartosc = resp[9] & 0x01"]:::gray
    G --> H["Serial: FC01 Odczyt"]:::green

    I["zapiszCoil(adres, wartosc)"]:::blue
    I --> J["budujNaglowek(req, 0x05, adres,<br/>wartosc ? 0xFF00 : 0x0000)"]:::teal
    J --> K["wyslijPakiet(req, 12)"]:::teal
    K --> L["Serial: FC05 Zapis"]:::green

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
```

---

## Diagram — `app/app` + `main`

Logika aplikacji i główna pętla.

```mermaid
flowchart TD
    START([setup]):::gray --> INIT["inicjalizujEthernet()"]:::blue
    INIT --> C{"OK?"}:::purple
    C -- nie --> ERR["while(true)"]:::red
    C -- tak --> LOOP

    LOOP([loop]):::gray --> TIMER{"millis - t<br/>>= 3000ms?"}:::purple
    TIMER -- nie --> LOOP
    TIMER -- tak --> P["polacz()"]:::blue
    P --> CONN{"connected?"}:::purple
    CONN -- nie --> LOOP
    CONN -- tak --> CYK["wykonajCykl()"]:::amber
    CYK --> READ["odczytajCoil(0, 1)"]:::teal
    READ --> TOG["toggle = !toggle"]:::gray
    TOG --> WRITE["zapiszCoil(0, toggle)"]:::teal
    WRITE --> RESET["t = millis()"]:::gray
    RESET --> LOOP

    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef amber fill:#FAEEDA,stroke:#854F0B,color:#633806
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```

# ESP32 + W5500 + OLED + Enkoder — Modbus Tester
 
Narzędzie do testowania Modbus TCP — tryb Master i Slave, konfiguracja przez enkoder i wyświetlacz OLED.
 
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
 
### OLED SSD1306 → ESP32 (HSPI)
 
| OLED | ESP32   | Opis |
|------|---------|------|
| GND  | GND     | Masa |
| VCC  | 3.3V    | Zasilanie |
| D0   | GPIO 14 | Zegar SPI |
| D1   | GPIO 13 | Dane |
| RES  | GPIO 26 | Reset |
| DC   | GPIO 27 | Data/Command |
| CS   | GPIO 15 | Chip Select |
 
### Enkoder → ESP32
 
| Enkoder | ESP32   | Opis |
|---------|---------|------|
| CLK     | GPIO 32 | Obrót A |
| DT      | GPIO 33 | Obrót B |
| SW      | GPIO 25 | Przycisk |
| +       | 3.3V    | Zasilanie |
| GND     | GND     | Masa |
 
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
- [x] Etap 2 — OLED hello world
- [x] Etap 3 — Enkoder, odczyt obrotów i przycisku
- [x] Etap 4 — Menu nawigacja OLED + enkoder
- [ ] Etap 5 — Konfiguracja IP, Port, Slave ID przez menu
- [ ] Etap 6 — Integracja konfiguracji z Modbusem
- [ ] Etap 7 — Tryb Slave
 
---
 
## Etap 2 — OLED
 
Moduł `src/display/` obsługuje wyświetlacz SSD1306 przez SPI (HSPI).
 
> **Ważne:** OLED musi być inicjalizowany przed Ethernetem (W5500) — oba używają SPI i W5500 blokuje magistralę jeśli wystartuje pierwszy.
 
```mermaid
flowchart TD
    A["inicjalizujDisplay()"]:::blue --> B{"display.begin<br/>SSD1306_SWITCHCAPVCC"}:::purple
    B -- nie --> C["Serial: BLAD<br/>return false"]:::red
    B -- tak --> D["clearDisplay()<br/>setTextSize(1)<br/>setTextColor(WHITE)"]:::teal
    D --> E["Serial: OK<br/>return true"]:::green
 
    F["pokazTekst(linia1, linia2)"]:::blue --> G["clearDisplay()"]:::teal
    G --> H["setCursor(0,0)<br/>println(linia1)"]:::teal
    H --> I{"linia2?"}:::purple
    I -- tak --> J["println(linia2)"]:::teal
    I -- nie --> K["display()"]:::teal
    J --> K
 
    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef red fill:#FCEBEB,stroke:#A32D2D,color:#791F1F
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
| `TypMenu` | Enum — identyfikator aktywnego podmenu (`GLOWNE`, `MASTER_POLACZENIE`, `MASTER_PAKIET`, `SLAVE`) |
| `PozycjaMenu` | Jeden wiersz na OLEDzie — nazwa, cel nawigacji, dostępność |
| `KonfiguracjaMaster` | Parametry połączenia: IP, Port, Slave ID, FC, adres, wartość |
| `StanMenu` | Aktualny stan całego menu — aktywne podmenu, pozycja kursora, konfiguracja |
 
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
    ├── Połączenie
    │   ├── IP własne
    │   ├── Port
    │   └── Slave ID
    └── Rejestry
        ├── Coils
        ├── Discrete Inputs
        ├── Input Registers
        └── Holding Registers
```
 
### Sterowanie
 
| Akcja | Efekt |
|-------|-------|
| Obrót w prawo | Następna pozycja |
| Obrót w lewo | Poprzednia pozycja |
| Przycisk | Wejdź głębiej / zatwierdź |
| Ostatnia pozycja + obrót w lewo | Wstecz |
 
### Diagram — nawigacja (`menu.cpp`)
 
```mermaid
flowchart TD
    A["inicjalizujMenu()"]:::blue --> B["aktywne = GLOWNE<br/>kursor = 0"]:::teal
 
    C["aktualizujMenu(delta, przycisk)"]:::blue --> D["aktualnePositcje()<br/>pobierz tablicę i rozmiar"]:::teal
    D --> E{"delta != 0?"}:::purple
    E -- tak --> F["kursor += delta<br/>ogranicz 0..rozmiar-1"]:::teal
    E -- nie --> G{"przycisk?"}:::purple
    F --> G
    G -- nie --> H["return"]:::gray
    G -- tak --> I{"pozycja.dostepna?"}:::purple
    I -- nie --> H
    I -- tak --> J["aktywne = pozycja.cel<br/>kursor = 0"]:::green
 
    K["aktualnePositcje(rozmiar)"]:::blue --> L{"stan.aktywne?"}:::purple
    L -- MASTER_POLACZENIE --> M["return menuMasterPolaczenie"]:::teal
    L -- MASTER_PAKIET --> N["return menuMasterPakiet"]:::teal
    L -- default --> O["return menuGlowne"]:::teal
 
    classDef blue fill:#E6F1FB,stroke:#185FA5,color:#0C447C
    classDef purple fill:#EEEDFE,stroke:#534AB7,color:#3C3489
    classDef green fill:#EAF3DE,stroke:#3B6D11,color:#27500A
    classDef teal fill:#E1F5EE,stroke:#0F6E56,color:#085041
    classDef gray fill:#F1EFE8,stroke:#5F5E5A,color:#2C2C2A
```
 
---
 
## Etap 5 — Konfiguracja
 
> *do uzupełnienia*
 
---
 
## Etap 6 — Integracja Modbus
 
> *do uzupełnienia*
 
---
 
## Etap 7 — Tryb Slave
 
> *do uzupełnienia*