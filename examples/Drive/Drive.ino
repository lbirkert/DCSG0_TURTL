#include<DCSG0_TURTL.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);

  delay(200);
  Serial.println();
  
  flsh_init();
  navg_init();
  comm_init();
}

struct drive_t {
  float l;
  float r;
};

void loop() {
  navg_update();
  comm_update();

  while(comm_network.available()) {
    Serial.println("RECV");

    RF24NetworkHeader header;
    drive_t drive;
    comm_network.read(header, &drive, sizeof(drive));

    navg_l = drive.l;
    navg_r = drive.r;
  }
}