/**
 * @file OscMessage.h
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * messages.
 *
 * MAX_OSC_ADDRESS_PATTERN_LENGTH and MAX_NUMBER_OF_ARGUMENTS may be modified as
 * required by the user application.
 *
 * See http://opensoundcontrol.org/spec-1_0
 */

#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

//------------------------------------------------------------------------------
// Includes

#include "OscCommon.h"
#include "OscError.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Minimum size (number of bytes) of an OSC message as per the OSC
 * specification.  The size is 8 bytes which includes the terminating null
 * character.
 */
#define MIN_OSC_MESSAGE_SIZE (sizeof("/\0\0\0,\0\0"))

/**
 * @brief Maximum size (number of bytes) of an OSC message equal to the maximum
 * packet size permitted by the transport layer.
 */
#define MAX_OSC_MESSAGE_SIZE (MAX_TRANSPORT_SIZE)

/**
 * @brief Maximum string length (excludes terminating null characters) of an OSC
 * address pattern.  This value may be modified as required by the user
 * application.
 */
#define MAX_OSC_ADDRESS_PATTERN_LENGTH (64)

/**
 * @brief Maximum number of arguments that may be contained within an OSC
 * message.  This value may be modified as required by the user application.
 */
#define MAX_NUMBER_OF_ARGUMENTS (16)

/**
 * @brief Maximum length of an OSC type tag string (includes comma but not
 * terminating null characters).
 */
#define MAX_OSC_TYPE_TAG_STRING_LENGTH (1 + MAX_NUMBER_OF_ARGUMENTS)

/**
 * @brief Maximum combined size (number of bytes) of all arguments that may be
 * contained within an OSC message.  The calculation assumes the worst case of
 * and extra 4 null characters for both the OSC address pattern and the OSC type
 * tag string.
 */
#define MAX_ARGUMENTS_SIZE (MAX_OSC_MESSAGE_SIZE - (MAX_OSC_ADDRESS_PATTERN_LENGTH + 4) - (MAX_OSC_TYPE_TAG_STRING_LENGTH + 4))

/**
 * @brief OSC message structure.  Structure members are used internally and
 * should not be used by the user application.
 */
typedef struct {
    char oscAddressPattern[MAX_OSC_ADDRESS_PATTERN_LENGTH + 1]; // must be first member so that first byte of structure is equal to '/'.  Null terminated.
    char oscTypeTagString[MAX_OSC_TYPE_TAG_STRING_LENGTH + 1]; // includes comma.  Null terminated
    char arguments[MAX_ARGUMENTS_SIZE];
    size_t oscAddressPatternLength; // does not include null characters
    size_t oscTypeTagStringLength; // includes comma but not null characters
    size_t argumentsSize;
    unsigned int oscTypeTagStringIndex;
    unsigned int argumentsIndex;
} OscMessage;

/**
 * @brief OSC type tag string characters indicating argument type.
 */
typedef enum {
    OscTypeTagInt32 = 'i',
    OscTypeTagFloat32 = 'f',
    OscTypeTagString = 's',
    OscTypeTagBlob = 'b',
    OscTypeTagInt64 = 'h',
    OscTypeTagTimeTag = 't',
    OscTypeTagDouble = 'd',
    OscTypeTagAlternateString = 'S',
    OscTypeTagCharacter = 'c',
    OscTypeTagRgbaColour = 'r',
    OscTypeTagMidiMessage = 'm',
    OscTypeTagTrue = 'T',
    OscTypeTagFalse = 'F',
    OscTypeTagNil = 'N',
    OscTypeTagInfinitum = 'I',
    OscTypeTagBeginArray = '[',
    OscTypeTagEndArray = ']',
} OscTypeTag;

//------------------------------------------------------------------------------
// Function prototypes

// Message construction
OscError OscMessageInitialise(OscMessage * const oscMessage, const char * oscAddressPattern);
OscError OscMessageSetAddressPattern(OscMessage * const oscMessage, const char * oscAddressPattern);
OscError OscMessageAppendAddressPattern(OscMessage * const oscMessage, const char * appendedParts);
OscError OscMessageAddInt32(OscMessage * const oscMessage, const int32_t int32);
OscError OscMessageAddFloat32(OscMessage * const oscMessage, const float float32);
OscError OscMessageAddString(OscMessage * const oscMessage, const char * string);
OscError OscMessageAddBlob(OscMessage * const oscMessage, const char * const source, const size_t numberOfBytes);
OscError OscMessageAddInt64(OscMessage * const oscMessage, const uint64_t int64);
OscError OscMessageAddTimeTag(OscMessage * const oscMessage, const OscTimeTag oscTimeTag);
OscError OscMessageAddDouble(OscMessage * const oscMessage, const Double64 double64);
OscError OscMessageAddAlternateString(OscMessage * const oscMessage, const char * string);
OscError OscMessageAddCharacter(OscMessage * const oscMessage, const char asciiChar);
OscError OscMessageAddRgbaColour(OscMessage * const oscMessage, const RgbaColour rgbaColour);
OscError OscMessageAddMidiMessage(OscMessage * const oscMessage, const MidiMessage midiMessage);
OscError OscMessageAddBool(OscMessage * const oscMessage, const bool boolean);
OscError OscMessageAddNil(OscMessage * const oscMessage);
OscError OscMessageAddInfinitum(OscMessage * const oscMessage);
OscError OscMessageAddBeginArray(OscMessage * const oscMessage);
OscError OscMessageAddEndArray(OscMessage * const oscMessage);
size_t OscMessageGetSize(const OscMessage * const oscMessage);
OscError OscMessageToCharArray(const OscMessage * const oscMessage, size_t * const oscMessageSize, char * const destination, const size_t destinationSize);

// Message deconstruction
OscError OscMessageInitialiseFromCharArray(OscMessage * const oscMessage, const char * const source, const size_t size);
bool OscMessageIsArgumentAvailable(OscMessage * const oscMessage);
OscTypeTag OscMessageGetArgumentType(OscMessage * const oscMessage);
OscError OscMessageSkipArgument(OscMessage * const oscMessage);
OscError OscMessageGetInt32(OscMessage * const oscMessage, int32_t * const int32);
OscError OscMessageGetFloat32(OscMessage * const oscMessage, float * const float32);
OscError OscMessageGetString(OscMessage * const oscMessage, char * const destination, const size_t destinationSize);
OscError OscMessageGetBlob(OscMessage * const oscMessage, size_t * const blobSize, char * const destination, const size_t destinationSize);
OscError OscMessageGetInt64(OscMessage * const oscMessage, int64_t * const int64);
OscError OscMessageGetTimeTag(OscMessage * const oscMessage, OscTimeTag * const oscTimeTag);
OscError OscMessageGetDouble(OscMessage * const oscMessage, Double64 * const double64);
OscError OscMessageGetCharacter(OscMessage * const oscMessage, char * const character);
OscError OscMessageGetRgbaColour(OscMessage * const oscMessage, RgbaColour * const rgbaColour);
OscError OscMessageGetMidiMessage(OscMessage * const oscMessage, MidiMessage * const midiMessage);
OscError OscMessageGetArgumentAsInt32(OscMessage * const oscMessage, int32_t * const int32);
OscError OscMessageGetArgumentAsFloat32(OscMessage * const oscMessage, float * const float32);
OscError OscMessageGetArgumentAsString(OscMessage * const oscMessage, char * const destination, const size_t destinationSize);
OscError OscMessageGetArgumentAsBlob(OscMessage * const oscMessage, size_t * const blobSize, char * const destination, const size_t destinationSize);
OscError OscMessageGetArgumentAsInt64(OscMessage * const oscMessage, int64_t * const int64);
OscError OscMessageGetArgumentAsTimeTag(OscMessage * const oscMessage, OscTimeTag * const oscTimeTag);
OscError OscMessageGetArgumentAsDouble(OscMessage * const oscMessage, Double64 * const double64);
OscError OscMessageGetArgumentAsCharacter(OscMessage * const oscMessage, char * const character);
OscError OscMessageGetArgumentAsRgbaColour(OscMessage * const oscMessage, RgbaColour * const rgbaColour);
OscError OscMessageGetArgumentAsMidiMessage(OscMessage * const oscMessage, MidiMessage * const midiMessage);
OscError OscMessageGetArgumentAsBool(OscMessage * const oscMessage, bool * const boolean);

#endif

//------------------------------------------------------------------------------
// End of file
