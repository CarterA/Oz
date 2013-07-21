//
//  Flight Computer
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#include "Data.h"
#include "Environment.h"
#include "Geiger.h"
#include "Motion.h"

#include <Wire.h>
#include <SPI.h>
#include <DataFlash.h>
#include <BMP085.h>
#include <I2Cdev.h>
#include <ADXL345.h>
#include <HMC5883L.h>
#include <ITG3200.h>

// Definitions
long Baud    = 115200;
int  LEDPin  = 12;
int  ModePin = 19;

Datum datum;

void setup() {
  
  pinMode(LEDPin, OUTPUT);
  Wire.begin();
  setupDataFlash();
  
  if (digitalRead(ModePin) == HIGH) {
    
    // We are in data output mode! Let's hope everything worked.
    while (!Serial) {}
    Serial.begin(Baud);
    readAndPrintData();
    
    // Once we're done, sit and do nothing forever.
    while (1) {}
    
  }
  
  Serial1.begin(Baud);
  delay(4000);
  
  if (setupEnvironmentalSensors() != 0)
    Serial1.println("*** Environmental sensor initialization error ***");
  //if (setupMotionSensors() != 0)
  //  Serial1.println("*** Motion sensor initialization error ***");
  setupGeigerCounter();
  delay(1000);
  
}

void loop() {
  
  digitalWrite(LEDPin, HIGH);
  
  datum.time = millis();
  //getMotionData(&datum);
  getEnvironmentalData(&datum);
  getGeigerCounterData(&datum);
  recordDatum(datum);
  
  digitalWrite(LEDPin, LOW);
  
}

