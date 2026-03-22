#pragma once
#include <Arduino.h>

// zwraca true jeśli operacja się powiodła
bool zapiszCoil(uint16_t adres, uint16_t wartosc, uint8_t slaveID = 1);
bool odczytajCoil(uint16_t adres, uint16_t ilosc, uint8_t slaveID = 1);