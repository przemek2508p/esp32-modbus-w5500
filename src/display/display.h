#pragma once
#include <Adafruit_SSD1306.h>

bool inicjalizujDisplay();
void pokazTekst(const char* linia1, const char* linia2 = nullptr);