// ---- COMM ----

//      _/_/_/    _/_/    _/      _/  _/      _/   
//   _/        _/    _/  _/_/  _/_/  _/_/  _/_/    
//  _/        _/    _/  _/  _/  _/  _/  _/  _/     
// _/        _/    _/  _/      _/  _/      _/      
//  _/_/_/    _/_/    _/      _/  _/      _/       
                                                
// Communication system for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include "comm.h"
#include "logg.h"
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#define __LOGG_SCOPE__ "COMM"

constexpr uint16_t comm_addr_local = 0x0001;
constexpr uint16_t comm_addr_remote = 0x0000;
 
RF24 comm_radio(9, 10);
RF24Network comm_network(comm_radio);

void comm_init() {
  LOGG_INFO() P(F("Initializing...")) LOGG_END();

  while (!comm_radio.begin()) {
    LOGG_FATAL() P(F("Radio hardware not responding!")) LOGG_END();
  }

  comm_radio.setChannel(90);
  comm_radio.setPALevel(RF24_PA_MIN);
  comm_radio.setDataRate(RF24_250KBPS);
  comm_network.begin(comm_addr_local);
}

void comm_update() {
  LOGG_DEBUG() P(F("Update")) LOGG_END();

  comm_network.update();

  while (!comm_radio.isChipConnected()) {
    LOGG_FATAL() P(F("Lost connection to chip!")) LOGG_END();
    // TODO: reset sequence
  }

  while (comm_radio.failureDetected) {
    LOGG_FATAL() P(F("Failure detected!")) LOGG_END();
    // TODO: reset sequence
  }
}

// ---- COMM ----
