#include<DCSG0_TURTL.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);

  delay(200);
  Serial.println();
  
  flsh_init();
  navg_init();

  navg_l = 2;
  navg_r = 2;
}

uint32_t last_change = 0;

void loop() {
  navg_update();

  if(millis() - last_change < 400) return;

  last_change = millis();

  //navg_cf_dbg(&navg_l_cf);

  Serial.print("min:0,max:6,l:");
  Serial.print(navg_l_sp);
  Serial.print(",r:");
  Serial.println(navg_r_sp);
}
