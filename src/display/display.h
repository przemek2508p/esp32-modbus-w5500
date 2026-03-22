#pragma once
#include <Adafruit_SSD1306.h>
#include "../menu/menu.h"

bool inicjalizujDisplay();
void pokazTekst(const char* linia1, const char* linia2 = nullptr);
void pokazMenu(const PozycjaMenu* pozycje, uint8_t rozmiar, uint8_t kursor);