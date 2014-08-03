//
//  Flight Computer - Motion Sensors
//  CCD Curiosity Balloon Satellite Project
//  Copyright 2012 Carter Allen
//

struct Datum;

//
// setupMotionSensors()
//
// Returns 0 if successful, 1 if error(s) are encountered.
//
int setupMotionSensors();

void getMotionData(Datum *datum);

