#pragma once
#include <Arduino.h>
#include "../menu/menu.h"  // TrybEdycji i KonfiguracjaMaster są w menu.h

// Stan aktualnie edytowanego parametru
struct StanEdycji {
  TrybEdycji tryb    = TrybEdycji::BRAK; // co edytujemy
  uint8_t    oktet   = 0;                // aktualny oktet przy edycji IP (0-3)
  bool       aktywna = false;            // czy jesteśmy w trybie edycji
};

// Inicjalizacja konfiguratora — wywołaj raz w setup()
void inicjalizujKonfigurator();

// Wejdź w edycję wybranego parametru
void wejdzWEdycje(TrybEdycji tryb);

// Obsługa enkodera podczas edycji
// zwraca true jeśli edycja zakończona (zatwierdzono)
bool aktualizujEdycje(int delta, bool przycisk, KonfiguracjaMaster& cfg);

// Zwraca aktualny stan edycji
StanEdycji& pobierzStanEdycji();

// Formatuje IP do wyświetlenia z podświetlonym oktetem
// np. "[192].168.001.001"
void formatujIP(char* buf, const uint8_t ip[4], uint8_t aktywnyOktet);

// Formatuje wartość liczbową do stringa
void formatujWartosc(char* buf, uint16_t wartosc);