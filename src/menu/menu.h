#pragma once
#include <Arduino.h>

// Identyfikatory podmenu — określają które menu jest aktualnie aktywne
enum class TypMenu {
  GLOWNE,             // ekran startowy z wyborem Master/Slave
  MASTER_POLACZENIE,  // ustawienia IP, Port, Slave ID
  MASTER_PAKIET,      // ustawienia FC, adres rejestru, wartość
  SLAVE               // wkrótce
};

// Pojedyncza pozycja w menu — jeden wiersz na OLEDzie
struct PozycjaMenu {
  const char* nazwa;   // tekst wyświetlany na ekranie
  TypMenu     cel;     // dokąd prowadzi wciśnięcie przycisku
  bool        dostepna; // false = pozycja zablokowana ("wkrótce")
};

// Parametry konfiguracji trybu Master
struct KonfiguracjaMaster {
  uint8_t  ip[4]   = {192, 168, 1, 1}; // IP serwera Modbus
  uint16_t port    = 502;               // port TCP
  uint8_t  slaveID = 1;                 // ID urządzenia Slave
  uint8_t  fc      = 0x01;             // Function Code (domyślnie FC01 = Coils)
  uint16_t adres   = 0;                // adres rejestru
  uint16_t wartosc = 0;                // wartość do zapisu
};

// Aktualny stan całego menu — gdzie jest kursor i jakie są ustawienia
struct StanMenu {
  TypMenu            aktywne = TypMenu::GLOWNE; // aktualnie widoczne podmenu
  uint8_t            kursor  = 0;               // indeks podświetlonej pozycji
  KonfiguracjaMaster master;                    // konfiguracja mastera
};

// Inicjalizacja menu — wywołaj raz w setup()
void inicjalizujMenu();

// Aktualizacja stanu menu na podstawie enkodera
// delta   — kierunek obrotu: +1 w prawo, -1 w lewo
// przycisk — true jeśli wciśnięto przycisk
void aktualizujMenu(int delta, bool przycisk);

// Zwraca referencję do aktualnego stanu menu
// referencja (&) = dostęp do oryginału, nie kopii
StanMenu& pobierzStan();

// Zwraca aktualną tablicę pozycji menu i jej rozmiar
const PozycjaMenu* pobierzPozycjeMenu(uint8_t& rozmiar);