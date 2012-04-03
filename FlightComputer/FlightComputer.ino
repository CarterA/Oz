#include <Wire.h>
#include <ADXL345.h>  // Accelerometer
#include <HMC5883L.h> // Magnetometer
#include <ITG3200.h>  // Gyroscope
#include <BMP085.h> // Barometer
#include <OneWire.h>
#include <DallasTemperature.h>

ADXL345 accel;
HMC5883L mag;
ITG3200 gyro;
BMP085 baro;

// Definitions
int    OneWireBus         = 2;
int    HIH4030Pin         = 0;
float  HIH4030Voltage     = 3.3;
float  ZeroPercentVoltage = 0.8;
int    BMP085BasePressure = 101320;

// OneWire & Dallas Temperature Sensor setup
OneWire oneWire(OneWireBus);
DallasTemperature tempSensors(&oneWire);
unsigned long tempDelay;

void setupAccelerometer();
void setupMagnetometer();
void setupGyroscope();
void setupTemperatureSensors();
void setupBarometer();
void writeAccelerometerData();
void writeMagnetometerData();
void writeGyroscopeData();
void writeTemperatureData();
void writeBarometerData();
void writeHumidityData();

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println();
  Serial.println("-+-+- Oz2 -+-+-");
  Serial.println("ms,accX,accY,accZ,magX,magY,magZ,gyrX,gyrY,gyrZ,gyrTem,barTem,barPres,barAlt,tem1,tem2,hum");
  Wire.begin();
  setupAccelerometer();
  setupMagnetometer();
  setupGyroscope();
  setupTemperatureSensors();
  setupBarometer();
  delay(1000);
}

void loop() {
  
  unsigned long startTime = millis();
  tempSensors.requestTemperatures();
  Serial.print(startTime);
  Serial.print(",");
  writeAccelerometerData();
  Serial.print(",");
  writeMagnetometerData();
  Serial.print(",");
  writeGyroscopeData();
  Serial.print(",");
  writeBarometerData();
  Serial.print(",");
  
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  if (elapsedTime < tempDelay)
    delay(tempDelay - elapsedTime);
  writeTemperatureData();
  
  Serial.print(",");
  writeHumidityData();
  
  Serial.println();
  delay(100);
  
}

void setupAccelerometer() {
  accel = ADXL345(0x53);
  if(!accel.EnsureConnected())
    Serial.println("Could not connect to ADXL345.");
  accel.SetRange(16, true);
  accel.EnableMeasurements();
}

void setupMagnetometer() {
  mag = HMC5883L();
  if(!mag.EnsureConnected())
    Serial.println("Could not connect to HMC5883L.");
  mag.SetScale(1.3);
  mag.SetMeasurementMode(Measurement_Continuous);
}

void setupGyroscope() {
  gyro = ITG3200();
  gyro.init(ITG3200_ADDR_AD0_LOW);
}

void setupTemperatureSensors() {
  tempSensors.setWaitForConversion(false);
  uint8_t resolution = tempSensors.getResolution();
  switch (resolution) {
    case 9:
      tempDelay = 94;
      break;
    case 10:
      tempDelay = 188;
      break;
    case 11:
      tempDelay = 375;
      break;
    case 12:
    default:
      tempDelay = 750;
      break;
  }
  tempSensors.begin();
}

void setupBarometer() {
  baro.begin();
}

void writeAccelerometerData() {
  if(!accel.IsConnected) return;
  AccelerometerScaled raw = accel.ReadScaledAxis();
  Serial.print(raw.XAxis);
  Serial.print(",");   
  Serial.print(raw.YAxis);
  Serial.print(",");
  Serial.print(raw.ZAxis);
}

void writeMagnetometerData() {
  if(!mag.IsConnected) return;
  MagnetometerRaw raw = mag.ReadRawAxis();
  Serial.print(raw.XAxis);
  Serial.print(",");   
  Serial.print(raw.YAxis);
  Serial.print(",");
  Serial.print(raw.ZAxis);
}

void writeGyroscopeData() {
  static float xyz[3];
  static float temp;
  gyro.readGyro(xyz);
  gyro.readTemp(&temp);
  Serial.print(xyz[0]);
  Serial.print(",");
  Serial.print(xyz[1]);
  Serial.print(",");
  Serial.print(xyz[2]);
  Serial.print(",");
  Serial.print(temp);
}

void writeTemperatureData() {
  Serial.print(tempSensors.getTempCByIndex(0));
  Serial.print(",");
  Serial.print(tempSensors.getTempCByIndex(1));
}

void writeBarometerData() {
  Serial.print(baro.readTemperature());
  Serial.print(",");
  Serial.print(baro.readPressure());
  Serial.print(",");
  Serial.print(baro.readAltitude(BMP085BasePressure));
}

void writeHumidityData() {
  float temp = tempSensors.getTempCByIndex(0);
  float rawVal = analogRead(HIH4030Pin);
  float voltage = rawVal/1023.0 * HIH4030Voltage;
  float sensorRH = 161.0 * voltage/HIH4030Voltage - 25.8;
  float trueRH = sensorRH/(1.0546 - 0.00216 * temp);
  Serial.print(trueRH);
}
