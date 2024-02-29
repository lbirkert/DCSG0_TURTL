#pragma once
#ifndef GYRO_H
#define GYRO_H

// --- GYRO ----

//      _/_/_/  _/      _/  _/_/_/      _/_/    
//   _/          _/  _/    _/    _/  _/    _/   
//  _/  _/_/      _/      _/_/_/    _/    _/    
// _/    _/      _/      _/    _/  _/    _/     
//  _/_/_/      _/      _/    _/    _/_/        
                                             
// Gyroscope interface for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include <Adafruit_MPU6050.h>

extern Adafruit_MPU6050 gyro;

extern float gyro_norm[3];
extern float gyro_accel[3];

void gyro_init();
void gyro_calib();
void gyro_update();

// ---- GYRO ----

#endif
