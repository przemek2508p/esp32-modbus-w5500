#pragma once
#include <Arduino.h>

enum class TypMenu {
  GLOWNE,
  MASTER_POLACZENIE,
  MASTER_PAKIET,
  SLAVE
};

enum class TrybEdycji {
  BRAK,
  IP,
  PORT,
  SLAVE_ID,
  FC,
  ADRES,
  WARTOSC
};

// Wynik aktualizacji menu — co się stało po obrocie/kliknięciu
enum class WynikMenu {
  BRAK,
  NAWIGACJA,
  WEJSCIE_EDYCJA,
  WEJSCIE_PODMENU
};

struct PozycjaMenu {
  const char* nazwa;
  TypMenu     cel;
  bool        dostepna;
  TrybEdycji  edycja;
};

struct KonfiguracjaMaster {
  uint8_t  ip[4]   = {192, 168, 1, 1};
  uint16_t port    = 502;
  uint8_t  slaveID = 1;
  uint8_t  fc      = 0x01;
  uint16_t adres   = 0;
  uint16_t wartosc = 0;
};

struct StanMenu {
  TypMenu            aktywne = TypMenu::GLOWNE;
  uint8_t            kursor  = 0;
  KonfiguracjaMaster master;
};

void inicjalizujMenu();
WynikMenu aktualizujMenu(int delta, bool przycisk);
StanMenu& pobierzStan();
const PozycjaMenu* pobierzPozycjeMenu(uint8_t& rozmiar);