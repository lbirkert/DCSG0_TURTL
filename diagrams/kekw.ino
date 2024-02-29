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

#define CODENAME "DCSG0_TURTL"
#define VERSION "E17-02-2024"

#include <DCSG0_TURTL.h>

float ges_l = 0;
float ges_r = 0;
uint32_t meassurements_ts = 0;
uint32_t meassurements = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  delay(200);
  Serial.println();

  LOGG_INFO() P(F("CODENAME=\""CODENAME"\"")) LOGG_END();
  LOGG_INFO() P(F("VERSION=\""VERSION"\"")) LOGG_END();
  LOGG_INFO() P(F("(c) 2024 The DCS Project Authors")) LOGG_END();

  flsh_init();
  //comm_init();
  //gyro_init();
  navg_init();

  //gyro_calib();
}

void loop() {
  //comm_update();
  gyro_update();
  navg_update();

  if(millis() - meassurements_ts >= 10) {
    meassurements_ts = millis();


    //Serial.println("S:" + String(navg_l_sp) + ", " + String(navg_r_sp));

    ges_l += navg_l_sp;
    ges_r += navg_r_sp;
    meassurements++;

    if(meassurements >= 100) {
      navg_l_pw += 1;
      navg_r_pw += 1;

      if(navg_l_pw >= 255) {
        Serial.println("F");
        navg_stop();
        while(1);
      }

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
      ges_l = 0;
      ges_r = 0;
    }
  }
}

