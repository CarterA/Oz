//
//  Geiger Counter Firmware
//  Copyright 2012 Carter Allen
//  Based on the work of Aaron Weiss
//

const int Baud      = 9600;
const int LEDPin    = 5;     // Status LED blinks for each count
const int TimerLoad = 34000; //(256/8MHz)*(65536bits-34000)~=1.009s

volatile long counts = 0;

void incrementCount(void) {
  counts++;
  digitalWrite(LEDPin, HIGH);
}

ISR (TIMER1_OVF_vect) {
  TCNT1 = TimerLoad;
  Serial.println(counts);
  counts = 0;
}

void setup() {
  Serial.begin(Baud);
  noInterrupts();
  attachInterrupt(INT0, incrementCount, FALLING);
  TCCR1A = 0x00;
  TCCR1B |= (1<<CS12);
  TIMSK1 |= (1<<TOIE1);
  TCNT1 = TimerLoad; // (256/8MHz)*(65536bits-34000)~=1.009s
  pinMode(LEDPin, OUTPUT);
  interrupts();
  delay(1200);
}

void loop() {
  digitalWrite(LEDPin, LOW);
  delay(30);
}
