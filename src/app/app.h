#pragma once
#include "../menu/menu.h"

// Wyślij pakiet Modbus na podstawie konfiguracji z menu
// Zwraca true jeśli wysłano i odebrano odpowiedź
bool wyslijPakietModbus(KonfiguracjaMaster& cfg);