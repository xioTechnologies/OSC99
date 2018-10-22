/**
 * @file OscCommon.h
 * @author Seb Madgwick
 * @brief Definitions, types, and functions used throughout library.
 * See http://opensoundcontrol.org/spec-1_0
 */

#ifndef OSC_COMMON_H
#define OSC_COMMON_H

//------------------------------------------------------------------------------
// Includes

#include <float.h> // DBL_MANT_DIG
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions - Application/platform specific

/**
 * @brief Comment out this definition if the platform is big-endian.  For
 * example: Arduino, Atmel AVR, Microchip PIC, Intel x86-64 are little-endian.
 * See http://en.wikipedia.org/wiki/Endianness
 */
#define LITTLE_ENDIAN_PLATFORM

/**
 * @brief Maximum packet size permitted by the transport layer.  Reducing this
 * value will reduce the amount of memory required.
 */
#define MAX_TRANSPORT_SIZE (1472)

/**
 * @brief Comment out this definition to prevent the OscErrorGetMessage function
 * from providing detailed error messages.  This will reduce the amount of
 * memory required.
 */
#define OSC_ERROR_MESSAGES_ENABLED

//------------------------------------------------------------------------------
// Definitions - 32-bit argument types

/**
 * @brief 32-bit RGBA colour.
 * See http://en.wikipedia.org/wiki/RGBA_color_space
 */
typedef struct __attribute__((__packed__)) {
#ifdef LITTLE_ENDIAN_PLATFORM
    char alpha; // LSB
    char blue;
    char green;
    char red; // MSB
#else
    char red; // MSB
    char green;
    char blue;
    char alpha; // LSB
#endif
}
RgbaColour;

/**
 * @brief 4 byte MIDI message as described in OSC 1.0 specification.
 */
typedef struct __attribute__((__packed__)) {
#ifdef LITTLE_ENDIAN_PLATFORM
    char data2; // LSB
    char data1;
    char status;
    char portID; // MSB
#else
    char portID; // MSB
    char status;
    char data1;
    char data2; // LSB
#endif
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
 * @brief OSC time tag.  Same representation used by NTP timestamps.
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

//------------------------------------------------------------------------------
// Variable declarations

extern const OscTimeTag oscTimeTagZero;

//------------------------------------------------------------------------------
// Function prototypes

bool OscContentsIsMessage(const void * const oscContents);
bool OscContentsIsBundle(const void * const oscContents);

#endif

//------------------------------------------------------------------------------
// End of file
