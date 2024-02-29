#pragma once
#ifndef LOGG_H
#define LOGG_H

// ---- LOGG ----

//     _/          _/_/      _/_/_/    _/_/_/   
//    _/        _/    _/  _/        _/          
//   _/        _/    _/  _/  _/_/  _/  _/_/     
//  _/        _/    _/  _/    _/  _/    _/      
// _/_/_/_/    _/_/      _/_/_/    _/_/_/       

// Logging system for the DCS generation G0

// (c) 2024 - Lucas Birkert

#include <stdio.h>
#include <inttypes.h>
#include <Print.h>
#include <WString.h>
#include <Printable.h>

#define LEVEL_DEBUG 0
#define LEVEL_INFO 1
#define LEVEL_WARN 2
#define LEVEL_ERROR 3
#define LEVEL_FATAL 4

#define LEVEL_HALT LEVEL_FATAL

#define LOGG_BEGIN(x) logg_begin(__LOGG_SCOPE__, x);
#define LOGG_DEBUG(x) LOGG_BEGIN(LEVEL_DEBUG)
#define LOGG_INFO(x)  LOGG_BEGIN(LEVEL_INFO)
#define LOGG_WARN(x)  LOGG_BEGIN(LEVEL_WARN)
#define LOGG_ERROR(x) LOGG_BEGIN(LEVEL_ERROR)
#define LOGG_FATAL(x) LOGG_BEGIN(LEVEL_FATAL)
#define LOGG_END(x)   logg_end();

#define P(...) logg_print.print(__VA_ARGS__);

void logg_flush();
void logg_begin(const char scope[5], uint8_t = LEVEL_INFO);
void logg_end();

class LoggPrint: public Print {
public:
  virtual size_t write(uint8_t b);
  using Print::write;
};
extern LoggPrint    logg_print;


// ---- LOGG ----

#endif
