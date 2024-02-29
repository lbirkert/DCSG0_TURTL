// ---- NAVG ----

//     _/      _/    _/_/    _/      _/    _/_/_/   
//    _/_/    _/  _/    _/  _/      _/  _/          
//   _/  _/  _/  _/_/_/_/  _/      _/  _/  _/_/     
//  _/    _/_/  _/    _/    _/  _/    _/    _/      
// _/      _/  _/    _/      _/        _/_/_/       

// Navigation system for the DCS generation G0

// (c) 2024 - Lucas Birkert

#include "navg.h"
#include "logg.h"
#include <Arduino.h>

#define __LOGG_SCOPE__ "NAVG"

#define NAVG_L_EN 3
#define NAVG_L_IN1 2
#define NAVG_L_IN2 4
#define NAVG_L_SP A1

#define NAVG_R_EN 6
#define NAVG_R_IN1 5
#define NAVG_R_IN2 7
#define NAVG_R_SP A0


// Wheel radius (in cm)
constexpr float    NAVG_CONST_R  = 6.6 / 2;
// Distance between wheels (in cm)
constexpr float    NAVG_CONST_L  = 18;
// Time interval between position syncs (in us)
constexpr uint32_t NAVG_POS_SYNC = 50000;
// History buffer depth
constexpr uint8_t  NAVG_HR = 10;
// Time interval between history syncs (in us)
constexpr uint32_t NAVG_HR_SYNC = 50000;

// The speed the wheels should move at (in 1/s)
float    navg_l = 0;
float    navg_r = 0;

// Power to left and right motors (from -255 to 255, neg indicates reverse)
int16_t  navg_l_pw = 0;
int16_t  navg_r_pw = 0;
// Last encoder update timestamp (in us)
uint32_t navg_l_ts = 0;
uint32_t navg_r_ts = 0;
// Last encoder state
bool     navg_l_st = false;
bool     navg_r_st = false;
// The meassured speed of the wheels (in 1/s)
float    navg_l_sp = 0;
float    navg_r_sp = 0;

// Last history update timestamp (in us)
uint32_t navg_hr_ts = 0;
// History ring buffer of past speed values
float    navg_l_hr[NAVG_HR];
float    navg_r_hr[NAVG_HR];
// Position in history ring buffer
uint8_t  navg_l_hr_pos = 0;
uint8_t  navg_r_hr_pos = 0;

// Power speed configuration table for both wheels
navg_cf_t navg_l_cf;
navg_cf_t navg_r_cf;

// Last position update timestamp (in us)
uint32_t navg_pos_ts = 0;
// Vehicle position
float    navg_pos_x  = 100;
float    navg_pos_y  = 100;
float    navg_pos_a  = 0;

// Initialize the navigation system
void navg_init() {
  LOGG_INFO() P(F("Initializing...")) LOGG_END();

  pinMode(NAVG_L_EN, OUTPUT);
  pinMode(NAVG_L_IN1, OUTPUT);
  pinMode(NAVG_L_IN2, OUTPUT);
  pinMode(NAVG_L_SP, INPUT);

  pinMode(NAVG_R_EN, OUTPUT);
  pinMode(NAVG_R_IN1, OUTPUT);
  pinMode(NAVG_R_IN2, OUTPUT);
  pinMode(NAVG_R_SP, INPUT);
  
  // Initialize configuration table to empty. TODO: store and load from flash
  for(int i = 0; i < 26; i++) {
    navg_l_cf.v[i] = NAN;
    navg_r_cf.v[i] = NAN;
  }

  navg_l_cf.v[0] = 0;
  navg_r_cf.v[0] = 0;
}

// Update the motor state
void navg_update_motor() {
  digitalWrite(NAVG_L_IN1, navg_l_pw > 0);
  digitalWrite(NAVG_L_IN2, navg_l_pw < 0);
  analogWrite(NAVG_L_EN, abs(navg_l_pw));

  digitalWrite(NAVG_R_IN1, navg_r_pw > 0);
  digitalWrite(NAVG_R_IN2, navg_r_pw < 0);
  analogWrite(NAVG_R_EN, abs(navg_r_pw));
}

// Return power level by speed from configuration
int16_t navg_cf_get(navg_cf_t* cf, float sp) {
  int p_i = 0;
  for(int c_i = 1; c_i < 26; c_i++) {
    if(isnan(cf->v[c_i])) continue;

    float t = (cf->v[p_i] - sp) / (cf->v[p_i] - cf->v[c_i]);

    if(t >= 0 && t <= 1) {
      return ceil(10.0 * ((1.0 - t) * (float) p_i + t * (float) c_i));
    }

    p_i = c_i;
  }

  return 10 * (ceil((25 + p_i) / 2.0));
}

// Update configuration table
void navg_cf_set(navg_cf_t* cf, int16_t pw, float sp) {
  int c_i = pw / 10;
  int n_i = c_i + 1;
  
  if(c_i == 0) return;

  if(isnan(cf->v[c_i])) {
    cf->v[c_i] = sp;
    return;
  }
  
  if(isnan(cf->v[n_i])) {
    cf->v[n_i] = sp;
    return;
  }

  float t = (float) (pw % 10) / 10.0;

  float e = (1 - t) * cf->v[c_i] + t * cf->v[n_i];

  cf->v[c_i] = max(0.0, cf->v[c_i] + (sp - e));
  cf->v[n_i] = max(0.0, cf->v[n_i] + (sp - e));
}

// Print out configuration table (for debugging and presentation)
void navg_cf_dbg(navg_cf_t* cf) {
  int p_i = 0;
  for(int c_i = 1; c_i < 26; c_i++) {
    if(isnan(cf->v[c_i])) continue;

    auto total = (c_i - p_i) * 1;
    for(int i = 0; i < total; i += 1) {
      float t = (float) i / (float) total;
      float v = t * cf->v[c_i] + (1 - t) * cf->v[p_i];

      Serial.print("min:0,max:6,v:");
      Serial.println(v);
    }

    p_i = c_i;
  }
}

// Update the speed history
void navg_update_history() {
  uint32_t now = micros();

  if(now - navg_hr_ts <= NAVG_HR_SYNC) return;
  
  navg_hr_ts = now;

  navg_l_hr[navg_l_hr_pos] = navg_l_sp;
  navg_l_hr_pos = (navg_l_hr_pos + 1) % NAVG_HR;
  navg_r_hr[navg_r_hr_pos] = navg_r_sp;
  navg_r_hr_pos = (navg_r_hr_pos + 1) % NAVG_HR;

  float l_total = 0.0;
  float r_total = 0.0;
  for(int i = 0; i < NAVG_HR - 1; i++) {
    float l_curr = navg_l_hr[(navg_l_hr_pos + NAVG_HR - 1 - i) % NAVG_HR];
    float l_prev = navg_l_hr[(navg_l_hr_pos + NAVG_HR - 2 - i) % NAVG_HR];
    
    float r_curr = navg_r_hr[(navg_r_hr_pos + NAVG_HR - 1 - i) % NAVG_HR];
    float r_prev = navg_r_hr[(navg_r_hr_pos + NAVG_HR - 2 - i) % NAVG_HR];

    l_total += l_curr - l_prev;
    r_total += r_curr - r_prev;
  }

  if(abs(l_total) < 0.4) {
    navg_cf_set(&navg_l_cf, abs(navg_l_pw), abs(navg_l_sp));
  }
  
  if(abs(r_total) < 0.4) {
    navg_cf_set(&navg_r_cf, abs(navg_r_pw), abs(navg_r_sp));
  }
}

// Update speed
void navg_update_speed() {
  uint32_t now = micros();

  // (1000000 us / s) / (x us * 40) = n / s
  float abs_l_sp = 1000000.0 / (float) (40 * (now - navg_l_ts));
  float abs_r_sp = 1000000.0 / (float) (40 * (now - navg_r_ts));

  float l_sp = copysign(abs_l_sp, navg_l_sp);
  float r_sp = copysign(abs_r_sp, navg_r_sp);

  if(abs_l_sp < abs(navg_l_sp)) navg_l_sp = l_sp;
  if(abs_r_sp < abs(navg_r_sp)) navg_r_sp = r_sp;

  if(navg_l_st != digitalRead(NAVG_L_SP)) {
    navg_l_sp = abs(navg_l) < 10 ? l_sp : copysign(abs_l_sp, navg_l_pw);
    navg_l_ts = now;
    navg_l_st ^= 1;
  }

  if(navg_r_st != digitalRead(NAVG_R_SP)) {
    navg_r_sp = abs(navg_r) < 10 ? r_sp : copysign(abs_r_sp, navg_r_pw);
    navg_r_ts = now;
    navg_r_st ^= 1;
  }

  // TODO: think about moving into separate function
  // Power correction (DZK)
  navg_l_pw = copysign(navg_cf_get(&navg_l_cf, abs(navg_l)), navg_l);
  navg_r_pw = copysign(navg_cf_get(&navg_r_cf, abs(navg_r)), navg_r);
  //navg_l_pw = navg_cf_get(&navg_l_cf, navg_l);
}

// Update position
void navg_update_pos() {
  uint32_t now = micros();

  if(now - navg_pos_ts <= NAVG_POS_SYNC) return;
  
  const uint16_t dt_us = now - navg_pos_ts;
  const float    dt    = dt_us / 1000000.0;
  navg_pos_ts = now;

  const float w_l = navg_l_sp * PI;
  const float w_r = navg_r_sp * PI;


  // kinematic equations
  float va = (NAVG_CONST_R / NAVG_CONST_L) * (w_l - w_r);
  float da = dt * va;

  float dx, dy;
  if(va == 0) {
    float pos_fac = dt * (NAVG_CONST_R / 2) * (w_l + w_r);
    dx = pos_fac * cos(navg_pos_a);
    dy = pos_fac * sin(navg_pos_a);
  } else {
    float pos_fac = (NAVG_CONST_R / (2 * va)) * (w_l + w_r);
    dx = pos_fac * (sin(navg_pos_a + da) - sin(navg_pos_a));
    dy = pos_fac * (cos(navg_pos_a) - cos(navg_pos_a + da));
  }

  navg_pos_x += dx;
  navg_pos_y += dy;
  navg_pos_a += da;

  // Constrain angle between 0 and 2 PI
  navg_pos_a = fmod(navg_pos_a, PI * 2);
}

// Update the navigation system
void navg_update() {
  LOGG_DEBUG() P(F("Update")) LOGG_END();

  navg_update_speed();
  navg_update_pos();
  navg_update_history();
  navg_update_motor();
}

// Cut power to the motors
void navg_stop() {
  navg_l_pw = 0;
  navg_r_pw = 0;
  navg_update_motor();
}

// ---- NAVG ----
