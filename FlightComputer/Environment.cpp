//
//  Flight Computer - Environmental Sensors
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#include "Environment.h"
#include "Data.h"
#include "I2Cdev.h"
#include "BMP085.h"

#include <Arduino.h>

int TMP1Pin = A4;
int TMP2Pin = A5;

BMP085 barometer;

byte HIH6131Address = 0x27;

void readTemperature(int pin, int *temperature);

int setupEnvironmentalSensors() {
  
  int error = 0;
  
  // Enable pull-up resistors for both temperature sensors
  digitalWrite(TMP1Pin, LOW);
  digitalWrite(TMP2Pin, LOW);
  
  barometer.initialize();
  error = !barometer.testConnection();
  
  return error;
  
}

void getEnvironmentalData(Datum *datum) {
  
  readTemperature(TMP1Pin, &datum->temp1);
  readTemperature(TMP2Pin, &datum->temp2);
  
  barometer.setControl(BMP085_MODE_TEMPERATURE);
  delayMicroseconds(barometer.getMeasureDelayMicroseconds());
  datum->baroTemp = barometer.getTemperatureC();
  
  barometer.setControl(BMP085_MODE_PRESSURE_3);
  delayMicroseconds(barometer.getMeasureDelayMicroseconds());
  datum->pressure = barometer.getPressure();
  
  Wire.beginTransmission(HIH6131Address);
  Wire.endTransmission();
  delay(37);
  
  Wire.requestFrom((int)HIH6131Address, (int)4);
  byte humH = Wire.read();
  byte humL = Wire.read();
  byte tempH = Wire.read();
  byte tempL = Wire.read();
  Wire.endTransmission();
  
  // byte status = (humH >> 6) & 0x03;
  humH = humH & 0x3f;
  unsigned int rawHumidity = (((unsigned int)humH) << 8) | humL;
  unsigned int rawTemperature = (((unsigned int)tempH) << 8) | tempL;
  
  datum->humidity = (float)rawHumidity * 6.1e-3;
  datum->humTemp = (float)(rawTemperature/4) * 1.007e-2 - 40.0;
  
}

void readTemperature(int pin, int *temperature) {
  
  int voltage = analogRead(pin);
  *temperature = voltage;
  
}
