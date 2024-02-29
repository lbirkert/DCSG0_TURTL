// ---- LOGG ----

//     _/          _/_/      _/_/_/    _/_/_/   
//    _/        _/    _/  _/        _/          
//   _/        _/    _/  _/  _/_/  _/  _/_/     
//  _/        _/    _/  _/    _/  _/    _/      
// _/_/_/_/    _/_/      _/_/_/    _/_/_/       

// Logging system for the DCS generation G0

// (c) 2024 - Lucas Birkert

#include "logg.h"
#include "flsh.h"
#include "navg.h"

#define __LOGG_SCOPE__ "LOGG"

constexpr size_t logg_cap = 50;
char             logg_buf[logg_cap + 1];
size_t           logg_pos = 0;
uint8_t          logg_lvl = 0;

LoggPrint        logg_print = LoggPrint();


void logg_flush_flsh() {
  if(logg_lvl <= LEVEL_DEBUG) {
    logg_pos = 0;
    return; 
  }

  // TODO: find out why this goes to 0
  //Serial.println("SIZE");
  //Serial.println(flsh_logg_size);
  //delay(100);

  // Write message to flash
  const size_t avail = flsh_logg_pos - flsh_logg_size;
  if(logg_pos > avail) {
    // split up
    flsh.writeCharArray(flsh_logg_data + flsh_logg_pos, logg_buf, avail);
    flsh.writeCharArray(flsh_logg_data, logg_buf, logg_pos - avail);
  } else {
    flsh.writeCharArray(flsh_logg_data + flsh_logg_pos, logg_buf, logg_pos);
  }

  // Update head
  flsh_logg_pos = (flsh_logg_pos + logg_pos) % flsh_logg_size;
  flsh.writeULong(flsh_logg_head, flsh_logg_pos);
}

void logg_flush() {
  if(logg_pos == 0) return;

  if(flsh_available) logg_flush_flsh();
  
  logg_buf[logg_pos] = 0;
  logg_pos = 0;
  
  Serial.print(logg_buf);
}

void logg_begin(const char scope[5], uint8_t lvl) {
  logg_flush();
  
  logg_lvl = lvl;

  P(F("[")) P(millis()) P(F("] ")) P(scope) P(F("::"));

  switch (lvl) {
    case LEVEL_DEBUG: P(F("DEBUG: ")); break;
    case LEVEL_INFO:  P(F("INFO: "));  break;
    case LEVEL_WARN:  P(F("WARN: "));  break;
    case LEVEL_ERROR: P(F("ERROR: ")); break;
    case LEVEL_FATAL: P(F("FATAL: ")); break;
  }
}

void logg_halt() {
  LOGG_INFO() P(F("Exceeded maximum logging level, halting...\n")) logg_flush();

  // TODO: Implement proper halting
  
  navg_stop();

  while(1) delay(100);
}

void logg_end() {
  P(F("\n"));

  logg_flush();
  
  if(logg_lvl >= LEVEL_HALT) logg_halt();
}

size_t LoggPrint::write(uint8_t b) {
  if(logg_pos == logg_cap) logg_flush();

  logg_buf[logg_pos++] = b;
  return 1;
}

// ---- LOGG ----
