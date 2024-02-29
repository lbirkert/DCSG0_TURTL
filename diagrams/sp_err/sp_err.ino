//     ==========     ==========     @@@@@@@@@@@@@@@@              @@@@@@@@@@@@@           @@@@@@@@@@@@    
//     ============ ============     @@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@       @@@@@@@@@@@@@@@@@ 
//     =========================     @@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@@
//            ===========            @@@@@@@@    @@@@@@@@@    @@@@@@@@@     @@@@@@@@@  @@@@@@@@    @@@@@@@@
//               =====               @@@@@@@@      @@@@@@@@  @@@@@@@@@       @@@@@@@@  @@@@@@@@@           
//                ===                @@@@@@@@      @@@@@@@@@ @@@@@@@@                  @@@@@@@@@@@@@@@@    
//                                   @@@@@@@@       @@@@@@@@ @@@@@@@@                    @@@@@@@@@@@@@@@@@ 
//                ===                @@@@@@@@      @@@@@@@@@ @@@@@@@@                       @@@@@@@@@@@@@@@
//               ====                @@@@@@@@      @@@@@@@@  @@@@@@@@@       @@@@@@@@ @@@@@@@@    @@@@@@@@@
//            ===========            @@@@@@@@    @@@@@@@@@    @@@@@@@@@     @@@@@@@@@ @@@@@@@@@     @@@@@@@
//     =========================     @@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@@
//     ============ ============     @@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@ 
//     ==========     ==========     @@@@@@@@@@@@@@@@              @@@@@@@@@@@@@          @@@@@@@@@@@@@@   

//     (c) 2024 - The DCS Project Authors

#define CODENAME "DCSG0_TURTL_DIAGRAMS_SP_ERR"
#define VERSION "E19-02-2024"

#include <DCSG0_TURTL.h>

float ges_l = 0;
float ges_r = 0;
uint32_t meassurements_ts = 0;
uint32_t meassurements = 0;

float avg_l = NAN;
float avg_r = NAN;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  delay(200);
  Serial.println();

  LOGG_INFO() P(F("CODENAME=\""CODENAME"\"")) LOGG_END();
  LOGG_INFO() P(F("VERSION=\""VERSION"\"")) LOGG_END();
  LOGG_INFO() P(F("(c) 2024 The DCS Project Authors")) LOGG_END();

  flsh_init();
  navg_init();
}

void meassure_err() {
  ges_l += abs(avg_l - navg_l_sp);
  ges_r += abs(avg_r - navg_r_sp);

  meassurements++;

  if(meassurements < 100) return;

  ges_l /= (float) meassurements;
  ges_r /= (float) meassurements;
  
  Serial.print(F("D"));
  Serial.print(navg_l_pw);
  Serial.print(F(","));
  Serial.print(ges_l);
  Serial.print(F(","));
  Serial.print(ges_r);
  Serial.println();


  meassurements = 0;
  avg_l = NAN;
  avg_r = NAN;
  ges_l = 0;
  ges_r = 0;
  navg_l_pw += 1;
  navg_r_pw += 1;
  
  if(navg_l_pw <= 255) return;

  Serial.println("F");
  navg_stop();
  while(1);
}

void meassure_avg() {
  ges_l += navg_l_sp;
  ges_r += navg_r_sp;
  meassurements++;

  if(meassurements < 100) return;

  ges_l /= (float) meassurements;
  ges_r /= (float) meassurements;

  meassurements = 0;
  avg_l = ges_l;
  avg_r = ges_r;
  ges_l = 0;
  ges_r = 0;
}


void loop() {
  navg_update();

  if(millis() - meassurements_ts < 10) return;

  meassurements_ts = millis();
  
  if(isnan(avg_l)) meassure_avg();
  else meassure_err();
}
