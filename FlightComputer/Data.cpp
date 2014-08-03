//
//  Flight Computer - Data Structures and Recording
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#include "Data.h"
#include "DataFlash.h"

#include <SPI.h>

DataFlash flash;

int page = 0;
int bytesInBuffer = 0;

int totalPages = DF_45DB642_PAGES;
int pageSize   = DF_45DB642_PAGESIZE;

void writeBytesToBuffer(byte *bytes, int offset);
void readNextDatumFromFlash(Datum *datum);
char * stringFromDatum(Datum datum);

void setupDataFlash() {
  SPI.begin();
  flash.setup(21);
  delay(10);
  flash.begin();
}

void recordDatum(Datum datum) {
  
  byte *bytes = (byte *)&datum;
  
  writeBytesToBuffer(bytes, bytesInBuffer);
  bytesInBuffer += sizeof(bytes);
  if (bytesInBuffer == pageSize) {
    flash.bufferToPage(0, page);
    page++;
  }
  
  Serial1.println(stringFromDatum(datum));
  
}

void readAndPrintData() {
  
  flash.arrayRead(0);
  Datum datum;
  unsigned long startingTime = -1;
  while (1) {
    readNextDatumFromFlash(&datum);
    if (startingTime == (unsigned long)(-1))
      startingTime = datum.time;
    else if (startingTime == datum.time) {
      flash.end();
      return;
    }
    Serial.println(stringFromDatum(datum));
  }
  
}

void writeBytesToBuffer(byte *bytes, int offset) {
  
  flash.bufferWrite(0, offset);
  unsigned int length = sizeof(bytes);
  for (unsigned int i = 0; i < length; i++) {
    SPI.transfer(bytes[i]);
  }
  
}

void readNextDatumFromFlash(Datum *datum) {
  
  byte *buffer = (byte *)datum;
  int length = sizeof(Datum);
  for (int i = 0; i < length; i++) {
    buffer[i] = SPI.transfer(0xff);
  }
  
}

char * stringFromDatum(Datum datum) {
  
  //
  // String length calculation:
  //
  // Triplet:
  //   ((int) -> 6) * 3 = 18
  //
  // (unsigned long)time         -> 10
  // ((Triplet)acc, mag, gyr)    -> 54
  // ((float)baroTemp, pressure) -> 24
  // ((int)temp1, temp2)         -> 12
  // ((float)humidity, humTemp)  -> 24
  // (unsigned int)counts        -> 5
  // (commas)                    -> 16
  // (null byte)                 -> 1
  // Total                       -> 146
  // 
  
  char *string = (char *)malloc(sizeof(char) * 145);
  sprintf(string, "%.10lu,%.6d,%.6d,%.6d,%.6d,%.6d,%.6d,%.6d,%.6d,%.6d,%g,%g,%.6d,%.6d,%g,%g,%u",
          datum.time,
          datum.acc.x, datum.acc.y, datum.acc.z,
          datum.mag.x, datum.mag.y, datum.mag.z,
          datum.gyr.x, datum.gyr.y, datum.gyr.z,
          (double)datum.baroTemp, (double)datum.pressure,
          datum.temp1, datum.temp2,
          (double)datum.humidity, (double)datum.humTemp,
          datum.counts);
  return string;
  
}
