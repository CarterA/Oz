//
//  Flight Computer - Motion Sensors
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

#include "Motion.h"
#include "Data.h"
#include "I2Cdev.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "ITG3200.h"

ADXL345  acc;
HMC5883L mag;
ITG3200  gyr;

int setupMotionSensors() {
  
  int error = 0;
  
  // Initialization
  Wire.begin();
  acc.initialize();
  mag.initialize();
  gyr.initialize();
  
  // Verification
  if (!acc.testConnection() ||
      !mag.testConnection() ||
      !gyr.testConnection())
      error = 1;
  
  // Configuration
  acc.setRange(ADXL345_RANGE_16G);
  mag.setMode(HMC5883L_MODE_CONTINUOUS);
    
  return error;
  
}

void getMotionData(Datum *datum) {
  
  acc.getAcceleration(&datum->acc.x, &datum->acc.y, &datum->acc.z);
  mag.getHeading(&datum->mag.x, &datum->mag.y, &datum->mag.z);
  gyr.getRotation(&datum->gyr.x, &datum->gyr.y, &datum->gyr.z);
  
}
