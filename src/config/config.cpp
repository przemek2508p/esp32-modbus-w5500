#include "config.h"
#include "../menu/menu.h"
#include <stdio.h>

// ── Stan globalny edycji ─────────────────────────────────────────
static StanEdycji stanEdycji;

void inicjalizujKonfigurator() {
  stanEdycji.tryb    = TrybEdycji::BRAK;
  stanEdycji.oktet   = 0;
  stanEdycji.aktywna = false;
}

void wejdzWEdycje(TrybEdycji tryb) {
  stanEdycji.tryb    = tryb;
  stanEdycji.oktet   = 0;
  stanEdycji.aktywna = true;
}

// ── Formatowanie ─────────────────────────────────────────────────

// Zamienia tablicę 4 bajtów IP na string z podświetlonym oktetem
// np. "192.168.001.010" gdzie aktywny oktet jest w nawiasach
void formatujIP(char* buf, const uint8_t ip[4], uint8_t aktywnyOktet) {
  sprintf(buf, "%s%03d%s.%s%03d%s.%s%03d%s.%s%03d%s",
    aktywnyOktet == 0 ? "[" : "", ip[0], aktywnyOktet == 0 ? "]" : "",
    aktywnyOktet == 1 ? "[" : "", ip[1], aktywnyOktet == 1 ? "]" : "",
    aktywnyOktet == 2 ? "[" : "", ip[2], aktywnyOktet == 2 ? "]" : "",
    aktywnyOktet == 3 ? "[" : "", ip[3], aktywnyOktet == 3 ? "]" : ""
  );
}

void formatujWartosc(char* buf, uint16_t wartosc) {
  sprintf(buf, "%d", wartosc);
}

// ── Logika edycji ────────────────────────────────────────────────
bool aktualizujEdycje(int delta, bool przycisk, KonfiguracjaMaster& cfg) {

  switch (stanEdycji.tryb) {

    case TrybEdycji::IP:
      if (delta != 0) {
        // zmień wartość aktywnego oktetu, ogranicz 0-255
        int nowa = cfg.ip[stanEdycji.oktet] + delta;
        if (nowa < 0)   nowa = 0;
        if (nowa > 255) nowa = 255;
        cfg.ip[stanEdycji.oktet] = (uint8_t)nowa;
      }
      if (przycisk) {
        stanEdycji.oktet++;
        if (stanEdycji.oktet > 3) {
          // wszystkie 4 oktety zatwierdzone
          stanEdycji.aktywna = false;
          stanEdycji.tryb    = TrybEdycji::BRAK;
          return true; // edycja zakończona
        }
      }
      break;

    case TrybEdycji::PORT:
      if (delta != 0) {
        int nowa = cfg.port + delta;
        if (nowa < 1)     nowa = 1;
        if (nowa > 65535) nowa = 65535;
        cfg.port = (uint16_t)nowa;
      }
      if (przycisk) {
        stanEdycji.aktywna = false;
        stanEdycji.tryb    = TrybEdycji::BRAK;
        return true;
      }
      break;

    case TrybEdycji::SLAVE_ID:
      if (delta != 0) {
        int nowa = cfg.slaveID + delta;
        if (nowa < 1)   nowa = 1;
        if (nowa > 247) nowa = 247;
        cfg.slaveID = (uint8_t)nowa;
      }
      if (przycisk) {
        stanEdycji.aktywna = false;
        stanEdycji.tryb    = TrybEdycji::BRAK;
        return true;
      }
      break;

    case TrybEdycji::FC:
      if (delta != 0) {
        int nowa = cfg.fc + delta;
        if (nowa < 1) nowa = 1;
        if (nowa > 6) nowa = 6; // FC01-FC06
        cfg.fc = (uint8_t)nowa;
      }
      if (przycisk) {
        stanEdycji.aktywna = false;
        stanEdycji.tryb    = TrybEdycji::BRAK;
        return true;
      }
      break;

    case TrybEdycji::ADRES:
      if (delta != 0) {
        int nowa = cfg.adres + delta;
        if (nowa < 0)     nowa = 0;
        if (nowa > 65535) nowa = 65535;
        cfg.adres = (uint16_t)nowa;
      }
      if (przycisk) {
        stanEdycji.aktywna = false;
        stanEdycji.tryb    = TrybEdycji::BRAK;
        return true;
      }
      break;

    case TrybEdycji::WARTOSC:
      if (delta != 0) {
        int nowa = cfg.wartosc + delta;
        if (nowa < 0)     nowa = 0;
        if (nowa > 65535) nowa = 65535;
        cfg.wartosc = (uint16_t)nowa;
      }
      if (przycisk) {
        stanEdycji.aktywna = false;
        stanEdycji.tryb    = TrybEdycji::BRAK;
        return true;
      }
      break;

    default:
      break;
  }

  return false; // edycja trwa
}

StanEdycji& pobierzStanEdycji() {
  return stanEdycji;
}