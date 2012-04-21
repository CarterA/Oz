//
//  Geiger Counter Firmware
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//  Based on the work of Aaron Weiss
//

#include <TimerOne.h>

long Baud      = 9600;
int  LEDPin    = 5;      // Status LED blinks for each count
long CountTime = 1000000; // 1 second, in microseconds

volatile long counts = 0;

void incrementCount(void) {
  counts++;
  digitalWrite(LEDPin, HIGH);
}

void printCount(void) {
  noInterrupts();
  Serial.print(millis());
  Serial.print(",");
  Serial.println(counts);
  counts = 0;
  interrupts();
}

void setup() {
  Serial.begin(Baud);
  noInterrupts();
  attachInterrupt(INT0, incrementCount, FALLING);
  Timer1.initialize(CountTime);
  Timer1.attachInterrupt(printCount);
  pinMode(LEDPin, OUTPUT);
  interrupts();
  delay(6000);
}

void loop() {
  digitalWrite(LEDPin, LOW);
  delay(30);
}
