#pragma once
#ifndef COMM_H
#define COMM_H

// ---- COMM ----

//      _/_/_/    _/_/    _/      _/  _/      _/   
//   _/        _/    _/  _/_/  _/_/  _/_/  _/_/    
//  _/        _/    _/  _/  _/  _/  _/  _/  _/     
// _/        _/    _/  _/      _/  _/      _/      
//  _/_/_/    _/_/    _/      _/  _/      _/       
                                                
// Communication system for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include <RF24.h>
#include <RF24Network.h>

extern const uint16_t comm_addr_local;
extern const uint16_t comm_addr_remote;
 
extern RF24 comm_radio;
extern RF24Network comm_network;

void comm_init();
void comm_update();

// ---- COMM ----

#endif
