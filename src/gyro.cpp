// --- GYRO ----

//      _/_/_/  _/      _/  _/_/_/      _/_/    
//   _/          _/  _/    _/    _/  _/    _/   
//  _/  _/_/      _/      _/_/_/    _/    _/    
// _/    _/      _/      _/    _/  _/    _/     
//  _/_/_/      _/      _/    _/    _/_/        
                                             
// Gyroscope interface for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include "gyro.h"
#include "logg.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define __LOGG_SCOPE__ "GYRO"

Adafruit_MPU6050 gyro;

float gyro_norm[3];
float gyro_accel[3];

void gyro_init() {
  LOGG_INFO() P(F("Initializing...")) LOGG_END();

  while (!gyro.begin()) {
    LOGG_FATAL() P(F("Failed to find chip!")) LOGG_END();
  }

  gyro.setAccelerometerRange(MPU6050_RANGE_8_G);

  LOGG_INFO() P(F("Accelerometer range set to: "))
  switch (gyro.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:  P(F("+-2G"));  break;
    case MPU6050_RANGE_4_G:  P(F("+-4G"));  break;
    case MPU6050_RANGE_8_G:  P(F("+-8G"));  break;
    case MPU6050_RANGE_16_G: P(F("+-16G")); break;
  } LOGG_END();

  gyro.setGyroRange(MPU6050_RANGE_500_DEG);

  LOGG_INFO() P(F("Gyro range set to: "))
  switch (gyro.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:  P(F("+- 250 deg/s"));  break;
    case MPU6050_RANGE_500_DEG:  P(F("+- 500 deg/s"));  break;
    case MPU6050_RANGE_1000_DEG: P(F("+- 1000 deg/s")); break;
    case MPU6050_RANGE_2000_DEG: P(F("+- 2000 deg/s")); break;
  } LOGG_END();

  gyro.setFilterBandwidth(MPU6050_BAND_5_HZ);

  LOGG_INFO() P(F("Filter bandwidth set to: "))
  switch (gyro.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ: P(F("260 Hz")); break;
    case MPU6050_BAND_184_HZ: P(F("184 Hz")); break;
    case MPU6050_BAND_94_HZ:  P(F("94 Hz"));  break;
    case MPU6050_BAND_44_HZ:  P(F("44 Hz"));  break;
    case MPU6050_BAND_21_HZ:  P(F("21 Hz"));  break;
    case MPU6050_BAND_10_HZ:  P(F("10 Hz"));  break;
    case MPU6050_BAND_5_HZ:   P(F("5 Hz"));   break;
  } LOGG_END();
}

void gyro_calib() {
  LOGG_INFO() P(F("Calibrating, don't move...")) LOGG_END();

  constexpr int meassurements = 50;

  float accelAvgX = 0;
  float accelAvgY = 0;
  float accelAvgZ = 0;

  for(int i = 0; i < meassurements; i++) {
    sensors_event_t a, g, temp;
    gyro.getEvent(&a, &g, &temp);

    accelAvgX += a.acceleration.x;
    accelAvgY += a.acceleration.y;
    accelAvgZ += a.acceleration.z;

    delay(100);
  }

  gyro_norm[0] = accelAvgX / meassurements;
  gyro_norm[1] = accelAvgY / meassurements;
  gyro_norm[2] = accelAvgZ / meassurements;

  LOGG_INFO() P(F("Norm acceleration: X: ")) P(gyro_norm[0])
                               P(F(", Y: ")) P(gyro_norm[1])
                               P(F(", Z: ")) P(gyro_norm[2]) LOGG_END();

  float magnitude = sqrt(
    gyro_norm[0] * gyro_norm[0] + 
    gyro_norm[1] * gyro_norm[1] + 
    gyro_norm[2] * gyro_norm[2]
  );

  LOGG_INFO() P(F("Norm magnitude: ")) P(magnitude) LOGG_END();
}

void gyro_update() {
  LOGG_DEBUG() P(F("Update")) LOGG_END();

  sensors_event_t a, g, temp;
  gyro.getEvent(&a, &g, &temp);

  gyro_accel[0] = a.acceleration.x - gyro_norm[0];
  gyro_accel[1] = a.acceleration.y - gyro_norm[1];
  gyro_accel[2] = a.acceleration.z - gyro_norm[2];
}

// ---- GYRO ----
