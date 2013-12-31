//==============================================================
//  mikeWeatherStation
//
//  local copy of diag.h
//
//  Mike McPherson
//  December 2013
//==============================================================

#ifndef DIAG_H
#define DIAG_H

//=================================
// diagnostics/debug support
// define for diagnostics
// comment out for none
//=================================
#define ENABLE_DIAG 1

#define DIAGPORT_RX     5
#define DIAGPORT_TX     4

#ifdef ENABLE_DIAG
#define DIAGPRINT(...)          diagport.print(__VA_ARGS__)
#define DIAGPRINTLN(...)        diagport.println(__VA_ARGS__)
#define DIAGFLUSH(...)          diagport.flush(__VA_ARGS__)
#else
#define DIAGPRINT(...)
#define DIAGPRINTLN(...)
#define DIAGFLUSH(...)
#endif

#endif //  DIAG_H

