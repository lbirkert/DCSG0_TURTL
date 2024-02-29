// ---- FLSH ----

//     _/_/_/_/  _/          _/_/_/  _/    _/   
//    _/        _/        _/        _/    _/    
//   _/_/_/    _/          _/_/    _/_/_/_/     
//  _/        _/              _/  _/    _/      
// _/        _/_/_/_/  _/_/_/    _/    _/       

// Flash interface for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include "flsh.h"
#include "logg.h"
#include <SPIMemory.h>

#define __LOGG_SCOPE__ "FLSH"

SPIFlash flsh(8);
bool flsh_available = false;

constexpr uint32_t flsh_logg_head = 0;
constexpr uint32_t flsh_logg_data = flsh_logg_head + 4;
constexpr uint32_t flsh_logg_size = 4194304; // 4 MiB
          uint32_t flsh_logg_pos;


void flsh_init() {
  LOGG_INFO() P(F("Initializing...")) LOGG_END();

  while (flsh.error()) {
    LOGG_FATAL() P(flsh.error(VERBOSE)) LOGG_END();
  }

  uint32_t JEDEC;
  while (1) {
    flsh.begin();
    JEDEC = flsh.getJEDECID();
    if(JEDEC) break;

    LOGG_FATAL() P(F("Communication error!")) LOGG_END();
  }

  flsh_logg_pos = flsh.readULong(flsh_logg_head);
  flsh_available = true;
  
  LOGG_INFO() P(F("JEDEC ID: ")) P(JEDEC, HEX) LOGG_END();
  LOGG_INFO() P(F("Capacity: ")) P(flsh.getCapacity()) LOGG_END();
  LOGG_INFO() P(F("Max Pages: ")) P(flsh.getMaxPage()) LOGG_END();
}

// ---- FLSH ----
