//
//  Flight Computer - Environmental Sensors
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

struct Datum;

//
// setupEnvironmentalSensors()
//
// Returns 0 if successful, 1 if error(s) are encountered.
//
int setupEnvironmentalSensors();

//
// getEnvironmentalData()
//
// Returns an array of 4 float values. They are as follows:
// tmp1,tmp2,pressure,humidity in C, C, Pa, and %, respectively.
//
void getEnvironmentalData(Datum *datum);

