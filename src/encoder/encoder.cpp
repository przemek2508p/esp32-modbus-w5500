#include "encoder.h"

#define ENC_CLK 32
#define ENC_DT  33
#define ENC_SW  25

volatile int pozycja = 0;
volatile bool przyciskWcisniety = false;
volatile uint8_t ostatniStan = 0;

void IRAM_ATTR obslugaObrotu() {
  uint8_t clk = digitalRead(ENC_CLK);
  uint8_t dt  = digitalRead(ENC_DT);
  uint8_t stan = (clk << 1) | dt;

  if (stan == ostatniStan) return;

  if (ostatniStan == 0b10 && stan == 0b00) pozycja++;
  if (ostatniStan == 0b01 && stan == 0b00) pozycja--;

  ostatniStan = stan;
}

void IRAM_ATTR obslugaPrzycisku() {
  static uint32_t ostatniCzas = 0;
  uint32_t teraz = millis();
  if (teraz - ostatniCzas > 300) {
    przyciskWcisniety = true;
    ostatniCzas = teraz;
  }
}

void inicjalizujEnkoder() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  pinMode(ENC_SW,  INPUT_PULLUP);

  ostatniStan = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  attachInterrupt(digitalPinToInterrupt(ENC_CLK), obslugaObrotu,    CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_DT),  obslugaObrotu,    CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_SW),  obslugaPrzycisku, FALLING);

  Serial.println("[OK] Enkoder gotowy");
}

int pobierzPozycje() {
  return pozycja;
}

bool pobierzPrzycisk() {
  if (przyciskWcisniety) {
    przyciskWcisniety = false;
    return true;
  }
  return false;
}