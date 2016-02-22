/**
 * @file OscCommon.h
 * @author Seb Madgwick
 * @brief Definitions and types used throughout library.
 *
 * MAX_TRANSPORT_SIZE may be modified as required by the user application.
 *
 * @see http://opensoundcontrol.org/spec-1_0
 */

#ifndef OSC_COMMON_H
#define OSC_COMMON_H

//------------------------------------------------------------------------------
// Includes

#include <float.h> // DBL_MANT_DIG
#include <stdint.h> // int32_t, uint32_t, uint64_t

//------------------------------------------------------------------------------
// Definitions - Application/platform specific

/**
 * @brief Maximum packet size permitted by transport layer.  Defines the maximum
 * size of an OSC packet.  This value may be modified as required by the user
 * application.
 */
#define MAX_TRANSPORT_SIZE (1472)

/**
 * @brief Comment out this definition if the platform is big-endian.  For
 * example, Arduino, Atmel AVR, Microchip PIC, Intel x86-64 are little-endian.
 * @see http://en.wikipedia.org/wiki/Endianness
 */
#define LITTLE_ENDIAN_PLATFORM

//------------------------------------------------------------------------------
// Definitions - OSC contents

/**
 * @brief Contents of OSC packet or OSC bundle element.  Must be either an OSC
 * message or OSC bundle.
 */
typedef void OscContents;

/**
 * @brief Enumerated types of contents.  The first byte of the contents
 * unambiguously distinguishes between an OSC bundle and an OSC message.
 */
typedef enum {
    OscContentsTypeMessage = '/',
    OscContentsTypeBundle = '#',
} OscContentsType;

/**
 * @brief Macro that resolves as true if oscContents points to an OSC message.
 */
#define OSC_CONTENTS_IS_MESSAGE(oscContents) (*(char*) (oscContents) == OscContentsTypeMessage)

/**
 * @brief Macro that resolves as true if oscContents points to an OSC bundle.
 */
#define OSC_CONTENTS_IS_BUNDLE(oscContents) (*(char*) (oscContents) == OscContentsTypeBundle)

//------------------------------------------------------------------------------
// Definitions - 32-bit argument types

/**
 * @brief 32-bit RGBA colour.
 * @see http://en.wikipedia.org/wiki/RGBA_color_space
 */
typedef struct __attribute__((__packed__)) {
    char blue; // LSB
    char green;
    char red;
    char alpha; // MSB
}
RgbaColour;

/**
 * @brief 4 byte MIDI message as described in OSC 1.0 specification.
 */
typedef struct __attribute__((__packed__)) {
    char data2; // LSB
    char data1;
    char status;
    char portID; // MSB
}
MidiMessage;

/**
 * @brief Union of all 32-bit OSC argument types defined in OSC 1.0
 * specification.
 */
typedef union {
    int32_t int32;
    float float32;
    RgbaColour rgbaColour;
    MidiMessage midiMessage;

    struct __attribute__((__packed__)) {
#ifdef LITTLE_ENDIAN_PLATFORM
        char byte0; // LSB
        char byte1;
        char byte2;
        char byte3; // MSB
#else
        char byte3; // MSB
        char byte2;
        char byte1;
        char byte0; // LSB
#endif
    }
    byteStruct;
} OscArgument32;

//------------------------------------------------------------------------------
// Definitions - 64-bit argument types

/**
 * @brief OSC time-tag.  Same representation used by NTP timestamps.
 */
typedef union {
    uint64_t value;

    struct __attribute__((__packed__)) {
        uint32_t fraction;
        uint32_t seconds;
    }
    dwordStruct;

    struct __attribute__((__packed__)) {
#ifdef LITTLE_ENDIAN_PLATFORM
        char byte0; // LSB
        char byte1;
        char byte2;
        char byte3;
        char byte4;
        char byte5;
        char byte6;
        char byte7; // MSB
#else
        char byte7; // MSB
        char byte6;
        char byte5;
        char byte4;
        char byte3;
        char byte2;
        char byte1;
        char byte0; // LSB
#endif
    }
    byteStruct;
} OscTimeTag;

/**
 * @brief OSC time-tag value of zero.
 */
#define OSC_TIME_TAG_ZERO ((OscTimeTag){0})

/**
 * @brief 64-bit double.  Defined as double or long double depending on
 * platform.
 */
#if (DBL_MANT_DIG == 53)
typedef double Double64;
#else
typedef long double Double64; // use long double if double is not 64-bit
#endif

/**
 * @brief Union of all 64-bit OSC argument types defined in OSC 1.0
 * specification.
 */
typedef union {
    uint64_t int64;
    OscTimeTag oscTimeTag;
    Double64 double64;

    struct __attribute__((__packed__)) {
#ifdef LITTLE_ENDIAN_PLATFORM
        char byte0; // LSB
        char byte1;
        char byte2;
        char byte3;
        char byte4;
        char byte5;
        char byte6;
        char byte7; // MSB
#else
        char byte7; // MSB
        char byte6;
        char byte5;
        char byte4;
        char byte3;
        char byte2;
        char byte1;
        char byte0; // LSB
#endif
    }
    byteStruct;
} OscArgument64;

#endif

//------------------------------------------------------------------------------
// End of file
