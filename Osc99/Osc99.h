/**
 * @file Osc99.h
 * @author Seb Madgwick
 * @brief Main header file for the library.  This is the only file that needs to
 * be included when using the library.
 *
 * OSC99 is a portable ANSI C99 compliant OSC library developed for use with
 * embedded systems.  OSC99 implements the OSC 1.0 specification including all
 * optional argument types.  The library also includes a SLIP module for
 * encoding and decoding OSC packets via unframed protocols such as UART/serial
 * as required by the OSC the 1.1 specification.
 *
 * The following definitions may be modified in OscCommon.h as required by the
 * user application: LITTLE_ENDIAN_PLATFORM, MAX_TRANSPORT_SIZE,
 * OSC_ERROR_MESSAGES_ENABLED.
 *
 * See http://opensoundcontrol.org/spec-1_0
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
