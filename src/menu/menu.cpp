#include "menu.h"
#include "../config/config.h"

static const PozycjaMenu menuGlowne[] = {
  { "Master", TypMenu::MASTER_POLACZENIE, true,  TrybEdycji::BRAK },
  { "Slave",  TypMenu::SLAVE,             false, TrybEdycji::BRAK },
};
static const uint8_t rozmiarGlowne = 2;

static const PozycjaMenu menuMasterPolaczenie[] = {
  { "IP serwera", TypMenu::MASTER_POLACZENIE, true, TrybEdycji::IP       },
  { "Port",       TypMenu::MASTER_POLACZENIE, true, TrybEdycji::PORT     },
  { "Slave ID",   TypMenu::MASTER_POLACZENIE, true, TrybEdycji::SLAVE_ID },
  { "Dalej >",    TypMenu::MASTER_PAKIET,     true, TrybEdycji::BRAK     },
  { "< Wstecz",   TypMenu::GLOWNE,            true, TrybEdycji::BRAK     },
};
static const uint8_t rozmiarMasterPolaczenie = 5;

static const PozycjaMenu menuMasterPakiet[] = {
  { "Func Code", TypMenu::MASTER_PAKIET,     true, TrybEdycji::FC      },
  { "Adres",     TypMenu::MASTER_PAKIET,     true, TrybEdycji::ADRES   },
  { "Wartosc",   TypMenu::MASTER_PAKIET,     true, TrybEdycji::WARTOSC },
  { "Wyslij",    TypMenu::MASTER_PAKIET,     true, TrybEdycji::BRAK    },
  { "< Wstecz",  TypMenu::MASTER_POLACZENIE, true, TrybEdycji::BRAK    },
};
static const uint8_t rozmiarMasterPakiet = 5;

static StanMenu stan;

static const PozycjaMenu* aktualnePositcje(uint8_t& rozmiar) {
  switch (stan.aktywne) {
    case TypMenu::MASTER_POLACZENIE:
      rozmiar = rozmiarMasterPolaczenie;
      return menuMasterPolaczenie;
    case TypMenu::MASTER_PAKIET:
      rozmiar = rozmiarMasterPakiet;
      return menuMasterPakiet;
    default:
      rozmiar = rozmiarGlowne;
      return menuGlowne;
  }
}

void inicjalizujMenu() {
  stan.aktywne = TypMenu::GLOWNE;
  stan.kursor  = 0;
}

WynikMenu aktualizujMenu(int delta, bool przycisk) {
  uint8_t rozmiar = 0;
  const PozycjaMenu* pozycje = aktualnePositcje(rozmiar);

  if (delta != 0) {
    int nowy = (int)stan.kursor + delta;
    if (nowy < 0)        nowy = 0;
    if (nowy >= rozmiar) nowy = rozmiar - 1;
    stan.kursor = (uint8_t)nowy;
    return WynikMenu::NAWIGACJA;
  }

  if (przycisk) {
    const PozycjaMenu& wybrana = pozycje[stan.kursor];
    if (wybrana.dostepna) {
      if (wybrana.edycja != TrybEdycji::BRAK) {
        wejdzWEdycje(wybrana.edycja);
        return WynikMenu::WEJSCIE_EDYCJA;
      } else {
        stan.aktywne = wybrana.cel;
        stan.kursor  = 0;
        return WynikMenu::WEJSCIE_PODMENU;
      }
    }
  }

  return WynikMenu::BRAK;
}

StanMenu& pobierzStan() {
  return stan;
}

const PozycjaMenu* pobierzPozycjeMenu(uint8_t& rozmiar) {
  return aktualnePositcje(rozmiar);
}