#pragma once
#include <LiquidCrystal_I2C.h>
#include "../menu/menu.h"
#include "../config/config.h"

bool inicjalizujDisplay();
void pokazTekst(const char* linia1, const char* linia2 = nullptr);
void pokazMenu(const PozycjaMenu* pozycje, uint8_t rozmiar, uint8_t kursor);
void pokazEdycje(StanEdycji& edycja, KonfiguracjaMaster& master);
void wstrzymajDisplay();
void wznowDisplay();