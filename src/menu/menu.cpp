#include "menu.h"

// ── Definicje pozycji w każdym podmenu ──────────────────────────

static const PozycjaMenu menuGlowne[] = {
  { "Master",  TypMenu::MASTER_POLACZENIE, true  },
  { "Slave",   TypMenu::SLAVE,             false },
};
static const uint8_t rozmiarGlowne = 2;

static const PozycjaMenu menuMasterPolaczenie[] = {
  { "IP serwera", TypMenu::MASTER_POLACZENIE, true },
  { "Port",       TypMenu::MASTER_POLACZENIE, true },
  { "Slave ID",   TypMenu::MASTER_POLACZENIE, true },
  { "Dalej >",    TypMenu::MASTER_PAKIET,     true },
  { "< Wstecz",   TypMenu::GLOWNE,            true },
};
static const uint8_t rozmiarMasterPolaczenie = 5;

static const PozycjaMenu menuMasterPakiet[] = {
  { "Func Code", TypMenu::MASTER_PAKIET,     true },
  { "Adres",     TypMenu::MASTER_PAKIET,     true },
  { "Wartosc",   TypMenu::MASTER_PAKIET,     true },
  { "Wyslij",    TypMenu::MASTER_PAKIET,     true },
  { "< Wstecz",  TypMenu::MASTER_POLACZENIE, true },
};
static const uint8_t rozmiarMasterPakiet = 5;

// ── Stan globalny ────────────────────────────────────────────────
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

// ── API ──────────────────────────────────────────────────────────
void inicjalizujMenu() {
  stan.aktywne = TypMenu::GLOWNE;
  stan.kursor  = 0;
}

void aktualizujMenu(int delta, bool przycisk) {
  uint8_t rozmiar = 0;
  const PozycjaMenu* pozycje = aktualnePositcje(rozmiar);

  if (delta != 0) {
    int nowy = (int)stan.kursor + delta;
    if (nowy < 0)        nowy = 0;
    if (nowy >= rozmiar) nowy = rozmiar - 1;
    stan.kursor = (uint8_t)nowy;
  }

  if (przycisk) {
    const PozycjaMenu& wybrana = pozycje[stan.kursor];
    if (wybrana.dostepna) {
      stan.aktywne = wybrana.cel;
      stan.kursor  = 0;
    }
  }
}

StanMenu& pobierzStan() {
  return stan;
}

const PozycjaMenu* pobierzPozycjeMenu(uint8_t& rozmiar) {
  return aktualnePositcje(rozmiar);
}