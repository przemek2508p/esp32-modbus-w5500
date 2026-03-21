#pragma once
#include <Arduino.h>

void zapiszCoil(uint16_t adres, bool wartosc);
void odczytajCoil(uint16_t adres, uint16_t ilosc);