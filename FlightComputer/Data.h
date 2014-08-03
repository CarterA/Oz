//
//  Flight Computer - Data Structures and Recording
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#import <stdint.h>

struct Triplet {
  int x, y, z;
};

struct Datum {                 // Bits:
  unsigned long time;          // 32
  Triplet       acc, mag, gyr; // 16 * 3
  float         baroTemp;      // 32
  float         pressure;      // 32
  int           temp1, temp2;  // 16 * 2
  float         humidity;      // 32
  float         humTemp;       // 32
  unsigned int  counts;        // 16
};              // Total:      // 256

void setupDataFlash();

void recordDatum(Datum datum);

void readAndPrintData();
