//
//  Flight Computer
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2013 Carter Allen
//

#include <Wire.h>
#include <BMP085.h>

BMP085 baro;

long Baud = 115200;
long AirSampleLength = 120000;
int  PumpPin = 9;
int  TakenAirSample = 0;

void setupBarometer(void);

void setup() {
  pinMode(PumpPin, OUTPUT);
  Serial.begin(Baud);
  delay(4000);
  Serial.println();
  Serial.println("-+-+-+- Oz -+-+-+-");
  Wire.begin();
  setupBarometer();
  delay(1000);
}

void loop() {
  
  writeBarometerData();
  
  Serial.println();
  
}

void setupBarometer() {
  baro.begin();
}

void writeBarometerData() {
  Serial.print(baro.readTemperature());
  Serial.print(",");
  float pressure = baro.readPressure();
  Serial.print(pressure);
  
  if (!TakenAirSample && pressure <= 3119.0) {
    collectAirSample();
    TakenAirSample = 1;
  }
  
}

void collectAirSample() {
  Serial.println();
  Serial.print("Beginning air sample.");
  digitalWrite(PumpPin, HIGH);
  delay(100);
  digitalWrite(PumpPin, LOW);
  delay(120000);
  Serial.println("Ending air sample.");
  digitalWrite(PumpPin, HIGH);
  delay(100);
  digitalWrite(PumpPin, LOW);
}
