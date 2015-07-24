/**
 * @file Osc99.h
 * @author Seb Madgwick
 * @brief Main header file for library.  This is the only file that needs to be
 * included when using the library.
 *
 * OSC99 is a highly portable ANSI C99 compliant OSC library developed for use
 * with embedded systems.  OSC99 implements the OSC 1.0 specification including
 * all optional argument types.  The library also includes a SLIP  module for
 * encoding and decoding OSC packets via unframed protocols such as UART/serial.
 *
 * @see http://opensoundcontrol.org/spec-1_0
 */

#ifndef OSC99_H
#define OSC99_H

//------------------------------------------------------------------------------
// Includes

#ifdef __cplusplus
extern "C" {
#endif

#include "OscAddress.h"
#include "OscError.h"
#include "OscPacket.h"
#include "OscSlip.h"

#ifdef __cplusplus
}
#endif

#endif
//------------------------------------------------------------------------------
// End of file
