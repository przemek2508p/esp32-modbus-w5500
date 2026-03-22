#pragma once
#include <Arduino.h>

void budujNaglowek(byte* req, uint8_t fc, uint16_t adres, uint16_t wartosc, uint8_t slaveID = 1);
void wyslijPakiet(byte* req, uint8_t len);