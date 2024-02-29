#pragma once
#ifndef NAVG_H
#define NAVG_H

// ---- NAVG ----

//     _/      _/    _/_/    _/      _/    _/_/_/   
//    _/_/    _/  _/    _/  _/      _/  _/          
//   _/  _/  _/  _/_/_/_/  _/      _/  _/  _/_/     
//  _/    _/_/  _/    _/    _/  _/    _/    _/      
// _/      _/  _/    _/      _/        _/_/_/       

// Naviagation system for the DCS generation G0

// (c) 2024 - Lucas Birkert

#include <inttypes.h>

// Configuration table for wheel
struct navg_cf_t {
  float v[27];
};

// The speed the wheels should move at (in 1/s)
extern float  navg_l;
extern float  navg_r;

// Power to left and right motors (from -255 to 255, neg indicates reverse)
extern int16_t  navg_l_pw;
extern int16_t  navg_r_pw;

// Last encoder update timestamp (in us)
extern uint32_t navg_l_ts;
extern uint32_t navg_r_ts;
// Last encoder state
extern bool     navg_l_st;
extern bool     navg_r_st;
// The speed of the wheels (in 1/s)
extern float    navg_l_sp;
extern float    navg_r_sp;

// Last history update timestamp (in us)
extern uint32_t navg_hr_ts;
// History ring buffer of past speed values
extern float    navg_l_hr[];
extern float    navg_r_hr[];
// Position in history ring buffer
extern uint8_t  navg_l_hr_pos;
extern uint8_t  navg_r_hr_pos;

// Power speed configuration table for both wheels
extern navg_cf_t navg_l_cf;
extern navg_cf_t navg_r_cf;

// Last position update timestamp (in us)
extern uint32_t navg_pos_ts;
// Vehicle position
extern float    navg_pos_x;
extern float    navg_pos_y;
extern float    navg_pos_a;

// Update and query configuration table
int16_t navg_cf_get(navg_cf_t*, float);
int16_t navg_cf_set(navg_cf_t*, float);
void    navg_cf_dbg(navg_cf_t*);

// Initialize the navigation system
void navg_init();

// Update the motor state
void navg_update_motor();

// Update speed
void navg_update_speed();

// Update history
void navg_update_history();

// Update position
void navg_update_pos();

// Update the navigation system
void navg_update();

// Cut power to the motors
void navg_stop();

// ---- NAVG ----

#endif
