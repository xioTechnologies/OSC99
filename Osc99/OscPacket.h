/**
 * @file OscPacket.h
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * packets.
 * @see http://opensoundcontrol.org/spec-1_0
 */

#ifndef OSC_PACKET_H
#define OSC_PACKET_H

//------------------------------------------------------------------------------
// Includes

#include "OscBundle.h"
#include "OscCommon.h"
#include "OscError.h"
#include "OscMessage.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Maximum OSC packet size.  The OSC packet size is limited by the
 * maximum packet size permitted by the transport protocol.
 */
#define MAX_OSC_PACKET_SIZE (MAX_TRANSPORT_SIZE)

/**
 * @brief OSC packet structure.  Must be initialised using either
 * OscPacketInitialise,  OscPacketInitialiseFromContents, or
 * OscPacketInitialiseFromCharArray.
 */
typedef struct {
    char contents[MAX_OSC_PACKET_SIZE];
    size_t size;
    void ( *processMessage)(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
} OscPacket;

//------------------------------------------------------------------------------
// Function prototypes

#ifdef OSC_SEND_ENABLED

void OscPacketInitialise(OscPacket * const oscPacket);
OscError OscPacketInitialiseFromContents(OscPacket * const oscPacket, const OscContents * const oscContents);

#endif

#ifdef OSC_RECIEVE_ENABLED

OscError OscPacketInitialiseFromCharArray(OscPacket * const oscPacket, const char * const source, const size_t sourceSize);
OscError OscPacketProcessMessages(OscPacket * const oscPacket);

#endif

#endif

//------------------------------------------------------------------------------
// End of file
