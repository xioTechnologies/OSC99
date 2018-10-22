/**
 * @file OscSlip.c
 * @author Seb Madgwick
 * @brief Functions and structures for encoding and decoding OSC packets using
 * the SLIP protocol.
 * See http://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol
 */

//------------------------------------------------------------------------------
// Includes

#include "OscSlip.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

#define SLIP_END ((char)0xC0)
#define SLIP_ESC ((char)0xDB)
#define SLIP_ESC_END ((char)0xDC)
#define SLIP_ESC_ESC ((char)0xDD)

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Encodes an OSC packet as a SLIP packet.
 *
 * The OSC packet is encoded as a SLIP packet and written to the destination
 * address.  The size of the encoded SLIP packet is written to the
 * destinationSize address.  If the destination is too small to contain the
 * encoded SLIP packet then the written size will be 0.
 *
 * Example use:
 * @code
 * char slipPacket[1024];
 * size slipPacketSize;
 * OscSlipEncodePacket(&oscPacket, slipPacket, &slipPacketSize, sizeof(slipPacket));
 * @endcode
 *
 * @param oscPacket OSC packet to be encoded.
 * @param destination Destination address of the OSC SLIP packet.
 * @param destinationSize Size of the destination.
 * @return Error code (0 if successful).
 */
OscError OscSlipEncodePacket(const OscPacket * const oscPacket, size_t * const slipPacketSize, char * const destination, const size_t destinationSize) {
    *slipPacketSize = 0; // size will be 0 if function unsuccessful
    unsigned int encodedPacketSize = 0;
    unsigned int packetIndex;
    for (packetIndex = 0; packetIndex < oscPacket->size; packetIndex++) {
        if ((encodedPacketSize + 1) > destinationSize) {
            return OscErrorDestinationTooSmall; // error: destination too small
        }
        switch (oscPacket->contents[packetIndex]) {
            case SLIP_END:
                destination[encodedPacketSize++] = SLIP_ESC;
                destination[encodedPacketSize++] = SLIP_ESC_END;
                break;
            case SLIP_ESC:
                destination[encodedPacketSize++] = SLIP_ESC;
                destination[encodedPacketSize++] = SLIP_ESC_ESC;
                break;
            default:
                destination[encodedPacketSize++] = oscPacket->contents[packetIndex];
        }
    }
    destination[encodedPacketSize++] = SLIP_END;
    *slipPacketSize = encodedPacketSize;
    return OscErrorNone;
}

/**
 * @brief Initialises an OSC SLIP decoder structure.
 *
 * An OSC SLIP decoder structure must be initialised before use.  A
 * ProcessPacket function must be implemented within the application and
 * assigned to the OSC SLIP decoder structure after initialisation.
 *
 * Example use:
 * @code
 * void ProcessPacket(OscPacket * const oscPacket){
 * }
 *
 * void Main() {
 *     OscSlipDecoder oscSlipDecoder;
 *     OscSlipDecoderInitialise(&oscSlipDecoder);
 *     oscSlipDecoder.processPacket = ProcessPacket;
 * }
 * @endcode
 *
 * @param oscSlipDecoder Address OSC SLIP decoder structure.
 */
void OscSlipDecoderInitialise(OscSlipDecoder * const oscSlipDecoder) {
    oscSlipDecoder->bufferIndex = 0;
    oscSlipDecoder->processPacket = NULL;
}

/**
 * @brief Processes byte received within serial stream.
 *
 * This function should be called for each consecutive byte received within a
 * serial stream.  Each byte is added to SLIP decoder receive buffer.  If the
 * received byte is the last byte of a SLIP packet then the SLIP packet is
 * decoded and parsed to the application as an OSC packet via the ProcessPacket
 * function.  The decoded packet will be discarded if a ProcessPacket function
 * has not been assigned.
 *
 * Example use:
 * @code
 * while(MySerialDataReady()){
 *     OscSlipDecoderProcessByte(&oscSlipDecoder, MySerialGetByte());
 * }
 * @endcode
 *
 * @param oscSlipDecoder Address OSC SLIP decoder structure.
 * @param byte Byte received within serial stream.
 * @return Error code (0 if successful).
 */
OscError OscSlipDecoderProcessByte(OscSlipDecoder * const oscSlipDecoder, const char byte) {

    // Add byte to buffer
    oscSlipDecoder->buffer[oscSlipDecoder->bufferIndex] = byte;

    // Increment index with overflow
    if (++oscSlipDecoder->bufferIndex >= OSC_SLIP_DECODER_BUFFER_SIZE) {
        oscSlipDecoder->bufferIndex = 0;
        return OscErrorEncodedSlipPacketTooLong; // error: SLIP packet is too long
    }

    // Return if byte not END byte
    if (byte != SLIP_END) {
        return OscErrorNone;
    }

    // Reset index
    oscSlipDecoder->bufferIndex = 0;

    // Decode packet
    OscPacket oscPacket;
    OscPacketInitialise(&oscPacket);
    unsigned int index = 0;
    while (oscSlipDecoder->buffer[index] != SLIP_END) {
        if (oscSlipDecoder->buffer[index] == SLIP_ESC) {
            switch (oscSlipDecoder->buffer[++index]) {
                case SLIP_ESC_END:
                    oscPacket.contents[oscPacket.size++] = SLIP_END;
                    break;
                case SLIP_ESC_ESC:
                    oscPacket.contents[oscPacket.size++] = SLIP_ESC;
                    break;
                default:
                    return OscErrorUnexpectedByteAfterSlipEsc; // error: unexpected byte value
            }
        } else {
            oscPacket.contents[oscPacket.size++] = oscSlipDecoder->buffer[index];
        }
        if (oscPacket.size > MAX_OSC_PACKET_SIZE) {
            return OscErrorDecodedSlipPacketTooLong; // error: decoded packet too large
        }
        index++;
    }

    // Call user function
    if (oscSlipDecoder->processPacket == NULL) {
        return OscErrorCallbackFunctionUndefined; // error: user function undefined
    }
    oscSlipDecoder->processPacket(&oscPacket);
    return OscErrorNone;
}

/**
 * @brief Clears the SLIP decoder receive buffer.
 *
 * Example use:
 * @code
 * OscSlipDecoderClearBuffer(&oscSlipDecoder);
 * @endcode
 *
 * @param oscSlipDecoder Address OSC SLIP decoder structure.
 */
void OscSlipDecoderClearBuffer(OscSlipDecoder * const oscSlipDecoder) {
    oscSlipDecoder->bufferIndex = 0;
}

//------------------------------------------------------------------------------
// End of file
