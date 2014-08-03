//
//  Flight Computer - Geiger Counter
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#include "Geiger.h"
#include "Data.h"

#include <Arduino.h>

volatile long counts = 0;

void incrementCount();

void setupGeigerCounter() {
  attachInterrupt(INT1, incrementCount, RISING);
  interrupts();
}

void getGeigerCounterData(Datum *datum) {
  noInterrupts();
  datum->counts = counts;
  counts = 0;
  interrupts();
}

void incrementCount() {
  counts++;
}
