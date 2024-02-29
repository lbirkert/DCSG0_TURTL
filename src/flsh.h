#pragma once
#ifndef FLSH_H
#define FLSH_H

// ---- FLSH ----

//     _/_/_/_/  _/          _/_/_/  _/    _/   
//    _/        _/        _/        _/    _/    
//   _/_/_/    _/          _/_/    _/_/_/_/     
//  _/        _/              _/  _/    _/      
// _/        _/_/_/_/  _/_/_/    _/    _/       

// Flash interface for the DCS generation G0
                                                
// (c) 2024 - Lucas Birkert

#include <SPIMemory.h>

extern SPIFlash flsh;
extern bool flsh_available;

extern const uint32_t flsh_logg_head;
extern const uint32_t flsh_logg_data;
extern const uint32_t flsh_logg_size;
extern       uint32_t flsh_logg_pos;

void flsh_init();

// ---- FLSH ----

#endif
