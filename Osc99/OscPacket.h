/**
 * @file OscPacket.h
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * packets.
 * See http://opensoundcontrol.org/spec-1_0
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
 * maximum packet size permitted by the transport layer.
 */
#define MAX_OSC_PACKET_SIZE (MAX_TRANSPORT_SIZE)

/**
 * @brief OSC packet structure.  Structure members are used internally and
 * should not be used by the user application.
 */
typedef struct {
    char contents[MAX_OSC_PACKET_SIZE];
    size_t size;
    void ( *processMessage)(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
} OscPacket;

//------------------------------------------------------------------------------
// Function prototypes

void OscPacketInitialise(OscPacket * const oscPacket);
OscError OscPacketInitialiseFromContents(OscPacket * const oscPacket, const void * const oscContents);
OscError OscPacketInitialiseFromCharArray(OscPacket * const oscPacket, const char * const source, const size_t numberOfBytes);
OscError OscPacketProcessMessages(OscPacket * const oscPacket);

#endif

//------------------------------------------------------------------------------
// End of file
