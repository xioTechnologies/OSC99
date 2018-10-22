/**
 * @file OscPacket.c
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * packets.
 * See http://opensoundcontrol.org/spec-1_0
 */

//------------------------------------------------------------------------------
// Includes

#include "OscPacket.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Function prototypes

static OscError DeconstructContents(OscPacket * const oscPacket, const OscTimeTag * const oscTimeTag, const void * const oscContents, const size_t contentsSize);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises an OSC packet.
 *
 * An OSC packet must be initialised before use.  This function can be used to
 * initialise an OSC packet before writing to the contents and size members
 * directly and is typically of use when constructing an OSC packet from
 * received bytes.
 *
 * Example use:
 * @code
 * OscPacket oscPacket;
 * OscPacketInitialise(&oscPacket);
 * const char source[] = "/example\0\0\0\0,\0\0"; // string terminating null character is part of OSC message
 * unsigned int index = 0;
 * while(index++ < sizeof(source)) {
 *     oscPacket.contents[index] = source[index];
 * }
 * oscPacket.size = sizeof(source);
 * @endcode
 *
 * @param oscPacket OSC packet to be initialised.
 */
void OscPacketInitialise(OscPacket * const oscPacket) {
    oscPacket->size = 0;
    oscPacket->processMessage = NULL;
}

/**
 * @brief Initialises an OSC packet from either OSC message or OSC
 * bundle.
 *
 * An OSC packet must be initialised before use.  This function is used to
 * initialise an OSC packet from either OSC message or OSC  bundle and is
 * typically of use when constructing an OSC packet for transmission.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "/example");
 * OscPacket oscPacket;
 * OscPacketInitialiseFromContents(&oscPacket, &oscMessage);
 * @endcode
 *
 * @param oscPacket OSC packet to be initialised.
 * @param oscContents OSC message or OSC bundle.
 * @return Error code (0 if successful).
 */
OscError OscPacketInitialiseFromContents(OscPacket * const oscPacket, const void * const oscContents) {
    oscPacket->processMessage = NULL;
    if (OscContentsIsMessage(oscContents) == true) {
        return OscMessageToCharArray((OscMessage *) oscContents, &oscPacket->size, oscPacket->contents, MAX_OSC_PACKET_SIZE);
    }
    if (OscContentsIsBundle(oscContents) == true) {
        return OscBundleToCharArray((OscBundle *) oscContents, &oscPacket->size, oscPacket->contents, MAX_OSC_PACKET_SIZE);
    }
    return OscErrorInvalidContents; // error: invalid or uninitialised OSC contents
}

/**
 * @brief Initialises an OSC packet from byte array.
 *
 * An OSC packet must be initialised before use.  This function is used to
 * initialise an OSC packet from a byte array and is typically of use when
 * constructing an OSC packet from received bytes.
 *
 * Example use:
 * @code
 * OscPacket oscPacket;
 * const char source[] = "/example\0\0\0\0,\0\0"; // string terminating null character is part of OSC message
 * OscPacketInitialiseFromCharArray(&oscPacket, source, sizeof(source));
 * @endcode
 *
 * @param oscPacket OSC packet to be initialised.
 * @param source Byte array.
 * @param numberOfBytes Number of bytes in byte array.
 * @return Error code (0 if successful).
 */
OscError OscPacketInitialiseFromCharArray(OscPacket * const oscPacket, const char * const source, const size_t numberOfBytes) {
    oscPacket->size = 0;
    if (numberOfBytes > MAX_OSC_PACKET_SIZE) {
        return OscErrorPacketSizeTooLarge; // error: size exceeds maximum packet size
    }
    while (oscPacket->size < numberOfBytes) {
        oscPacket->contents[oscPacket->size] = source[oscPacket->size];
        oscPacket->size++;
    }
    oscPacket->processMessage = NULL;
    return OscErrorNone;
}

/**
 * @brief Processes the OSC packet to provide each OSC message contained within
 * the packet to the user application with the associated OSC time tag (if the
 * message is contained within a bundle).
 *
 * A ProcessMessage function must be implemented within the application and
 * assigned to the OSC packet structure after initialisation.  The
 * ProcessMessage function will be called for each OSC message found within the
 * OSC packet.
 *
 * Example use:
 * @code
 * void ProcessMessage(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {
 * }
 *
 * void Main() {
 *     OscPacket oscPacket;
 *     const char source[] = "/example\0\0\0\0,\0\0\0";
 *     OscPacketInitialiseFromCharArray(&oscPacket, source, sizeof(source) - 1);
 *     oscPacket.processMessage = ProcessPacket;
 *     OscPacketProcessMessages(&oscPacket);
 * }
 * @endcode
 *
 * @param oscPacket OSC packet to be processed.
 * @return Error code (0 if successful).
 */
OscError OscPacketProcessMessages(OscPacket * const oscPacket) {
    if (oscPacket->processMessage == NULL) {
        return OscErrorCallbackFunctionUndefined; // error: user function undefined
    }
    return DeconstructContents(oscPacket, NULL, oscPacket->contents, oscPacket->size);
}

/**
 * @brief Recursively deconstructs the OSC contents to provide each OSC message
 * to the user application with the associated OSC time tag (if the message is
 * contained within a bundle).
 *
 * This is an internal function and cannot be called by the user application.
 *
 * @param oscPacket OSC packet.
 * @param oscTimeTag OSC time tag of the bundle containing the OSC contents.
 * Must be NULL if the contents is not within an OSC bundle.
 * @param oscContents OSC contents to be deconstructed.
 * @param contentsSize Size of the OSC contents.
 * @return Error code (0 if successful).
 */
static OscError DeconstructContents(OscPacket * const oscPacket, const OscTimeTag * const oscTimeTag, const void * const oscContents, const size_t contentsSize) {
    if (contentsSize == 0) {
        return OscErrorContentsEmpty; // error: contents empty
    }

    // Contents is an OSC message
    if (OscContentsIsMessage(oscContents) == true) {
        OscMessage oscMessage;
        const OscError oscError = OscMessageInitialiseFromCharArray(&oscMessage, oscContents, contentsSize);
        if (oscError != OscErrorNone) {
            return oscError; // error: message initialisation failed
        }
        oscPacket->processMessage(oscTimeTag, &oscMessage);
        return OscErrorNone;
    }

    // Contents is an OSC bundle
    if (OscContentsIsBundle(oscContents) == true) {
        OscBundle oscBundle;
        OscError oscError = OscBundleInitialiseFromCharArray(&oscBundle, oscContents, contentsSize);
        if (oscError != OscErrorNone) {
            return oscError; // error: bundle initialisation failed
        }
        do {
            OscBundleElement oscBundleElement;
            if (OscBundleIsBundleElementAvailable(&oscBundle) == false) {
                break; // no more bundle elements
            }
            oscError = OscBundleGetBundleElement(&oscBundle, &oscBundleElement);
            if (oscError != OscErrorNone) {
                return oscError; // error: get bundle element failed
            }
            oscError = DeconstructContents(oscPacket, &oscBundle.oscTimeTag, oscBundleElement.contents, oscBundleElement.size.int32); // recursive deconstruction
            if (oscError != OscErrorNone) {
                return oscError; // error: contents deconstruction failed
            }
        } while (true);
        return OscErrorNone;
    }

    return OscErrorInvalidContents; // error: invalid or uninitialised contents
}

//------------------------------------------------------------------------------
// End of file
