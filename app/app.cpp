#include "app.h"
#include "../modbus/coils.h"
#include <Arduino.h>

void wykonajCykl() {
  static bool toggle = false;
  odczytajCoil(0, 1);
  toggle = !toggle;
  zapiszCoil(0, toggle);
  Serial.println("---");
}