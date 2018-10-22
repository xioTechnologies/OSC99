/**
 * @file OscMessage.c
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * messages.
 * See http://opensoundcontrol.org/spec-1_0
 */

//------------------------------------------------------------------------------
// Includes

#include <limits.h> // SCHAR_MAX
#include "OscMessage.h"
#include <string.h> // strlen

//------------------------------------------------------------------------------
// Function prototypes

static int TerminateOscString(char * const oscString, size_t * const oscStringSize, const size_t maxOscStringSize);

//------------------------------------------------------------------------------
// Functions - Message construction

/**
 * @brief Initialises an OSC message.
 *
 * An OSC message must be initialised before use.  The oscAddressPattern
 * argument must be a null terminated string of zero of more characters.  A
 * message may be initialised without an address pattern by parsing an
 * oscAddressPattern value of "".  This may be of use if the address pattern is
 * undetermined at the time of initialisation.  In which case, the address
 * pattern may be set later using OscMessageSetAddressPattern.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "/example/address/pattern");
 * @endcode
 *
 * @param oscMessage OSC message to be initialised.
 * @param oscAddressPattern OSC address pattern as null terminated string.
 * @return Error code (0 if successful).
 */
OscError OscMessageInitialise(OscMessage * const oscMessage, const char * oscAddressPattern) {
    oscMessage->oscAddressPattern[0] = '\0'; // null terminate string
    oscMessage->oscTypeTagString[0] = ',';
    oscMessage->oscTypeTagString[1] = '\0'; // null terminate string
    oscMessage->oscAddressPatternLength = 0;
    oscMessage->oscTypeTagStringLength = 1; // includes comma
    oscMessage->argumentsSize = 0;
    oscMessage->oscTypeTagStringIndex = 1; // skip comma
    oscMessage->argumentsIndex = 0;
    if (*oscAddressPattern != '\0') {
        return OscMessageSetAddressPattern(oscMessage, oscAddressPattern);
    }
    return OscErrorNone;
}

/**
 * @brief Sets the OSC address pattern of an OSC message.
 *
 * The oscAddressPattern argument must be a null terminated string of zero of
 * more characters.  The existing OSC address pattern will be overwritten.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "");
 * OscMessageSetAddressPattern(&oscMessage, "/example/address/pattern");
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param oscAddressPattern OSC address pattern as null terminated string.
 * @return Error code (0 if successful).
 */
OscError OscMessageSetAddressPattern(OscMessage * const oscMessage, const char * oscAddressPattern) {
    oscMessage->oscAddressPatternLength = 0;
    return OscMessageAppendAddressPattern(oscMessage, oscAddressPattern);
}

/**
 * @brief Appends OSC address pattern parts to the OSC address pattern of an
 * OSC message.
 *
 * The appendedParts argument must be a null terminated string.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "");
 * OscMessageAppendAddressPattern(&oscMessage, "/example");
 * OscMessageAppendAddressPattern(&oscMessage, "/address");
 * OscMessageAppendAddressPattern(&oscMessage, "/pattern");
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param appendedParts OSC pattern parts to be appended.
 * @return Error code (0 if successful).
 */
OscError OscMessageAppendAddressPattern(OscMessage * const oscMessage, const char * appendedParts) {
    if (*appendedParts != '/') {
        return OscErrorNoSlashAtStartOfMessage; // error: address must start with '/'
    }
    while (*appendedParts != '\0') {
        if (oscMessage->oscAddressPatternLength >= MAX_OSC_ADDRESS_PATTERN_LENGTH) {
            return OscErrorAddressPatternTooLong; // error: address pattern too long
        }
        oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength++] = *appendedParts++;
    }
    oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a 32-bit integer argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInt32(&oscMessage, 123);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int32 32-bit integer to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddInt32(OscMessage * const oscMessage, const int32_t int32) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInt32;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.int32 = int32;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds a 32-bit float argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddFloat32(&oscMessage, 3.14f);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param float32 32-bit float to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddFloat32(OscMessage * const oscMessage, const float float32) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagFloat32;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.float32 = float32;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds a string argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddString(&oscMessage, "Hello World!");
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param string String to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddString(OscMessage * const oscMessage, const char * string) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    size_t argumentsSize = oscMessage->argumentsSize; // local copy in case function returns error
    while (*string != '\0') {
        if (argumentsSize >= MAX_ARGUMENTS_SIZE) {
            return OscErrorArgumentsSizeTooLarge; // error: message full
        }
        oscMessage->arguments[argumentsSize++] = *string++;
    }
    if (TerminateOscString(oscMessage->arguments, &argumentsSize, MAX_ARGUMENTS_SIZE) != 0) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->argumentsSize = argumentsSize;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagString;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a blob (byte array) argument to an OSC message.
 *
 * Example use:
 * @code
 * const char source[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };
 * OscMessageAddBlob(&oscMessage, source, sizeof(source));
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param source Byte array to be added as argument.
 * @param numberOfBytes Number of bytes in byte array to be added as argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddBlob(OscMessage * const oscMessage, const char * const source, const size_t numberOfBytes) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32) + numberOfBytes) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    size_t argumentsSize = oscMessage->argumentsSize; // local copy in case function returns error
    OscArgument32 blobSize;
    blobSize.int32 = (int32_t) numberOfBytes;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte3;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte2;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte1;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte0;
    unsigned int sourceIndex;
    for (sourceIndex = 0; sourceIndex < numberOfBytes; sourceIndex++) {
        oscMessage->arguments[argumentsSize++] = source[sourceIndex];
    }
    while ((argumentsSize % 4) != 0) {
        if (argumentsSize >= MAX_ARGUMENTS_SIZE) {
            return OscErrorArgumentsSizeTooLarge; // error: message full
        }
        oscMessage->arguments[argumentsSize++] = 0;
    }
    oscMessage->argumentsSize = argumentsSize;
    oscMessage->oscTypeTagString[(oscMessage->oscTypeTagStringLength)++] = OscTypeTagBlob;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a 64-bit integer argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInt64(&oscMessage, 123);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int64 64-bit integer to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddInt64(OscMessage * const oscMessage, const uint64_t int64) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument64)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInt64;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument64 oscArgument64;
    oscArgument64.int64 = int64;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds an OSC time tag argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddTimeTag(&oscMessage, oscTimeTagZero);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param oscTimeTag OSC time tag to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddTimeTag(OscMessage * const oscMessage, const OscTimeTag oscTimeTag) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscTimeTag)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[(oscMessage->oscTypeTagStringLength)++] = OscTypeTagTimeTag;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds a 64-bit double argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddDouble(&oscMessage, 3.14);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param double64 64-bit double to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddDouble(OscMessage * const oscMessage, const Double64 double64) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument64)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagDouble;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument64 oscArgument64;
    oscArgument64.double64 = double64;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds an alternate string argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddAlternateString(&oscMessage, "Hello World!");
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param string String to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddAlternateString(OscMessage * const oscMessage, const char * string) {
    const OscError oscError = OscMessageAddString(oscMessage, string);
    if (oscError != OscErrorNone) {
        return oscError;
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength - 1] = OscTypeTagAlternateString;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a char argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddCharacter(&oscMessage, 'a');
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param asciiChar Character to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddCharacter(OscMessage * const oscMessage, const char asciiChar) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagCharacter;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = asciiChar;
    return OscErrorNone;
}

/**
 * @brief Adds a 32-bit RGBA colour argument to an OSC message.
 *
 * Example use:
 * @code
 * const RgbaColour rgbaColour = { 0x00, 0x00, 0x00, 0x00 };
 * OscMessageAddRgbaColour(&oscMessage, rgbaColour);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param rgbaColour 32-bit RGBA colour to be added as argument to the OSC
 * message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddRgbaColour(OscMessage * const oscMessage, const RgbaColour rgbaColour) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagRgbaColour;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.rgbaColour = rgbaColour;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds a 4 byte MIDI message argument to an OSC message.
 *
 * Example use:
 * @code
 * const MidiMessage midiMessage = { 0x00, 0x00, 0x00, 0x00 };
 * OscMessageAddMidiMessage(&oscMessage, midiMessage);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param midiMessage 4 byte MIDI message to be added as argument to the OSC
 * message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddMidiMessage(OscMessage * const oscMessage, const MidiMessage midiMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    if ((oscMessage->argumentsSize + sizeof (OscArgument32)) > MAX_ARGUMENTS_SIZE) {
        return OscErrorArgumentsSizeTooLarge; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagMidiMessage;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.midiMessage = midiMessage;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return OscErrorNone;
}

/**
 * @brief Adds a boolean argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddBool(&oscMessage, true);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param boolean Boolean to be added as argument to the OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddBool(OscMessage * const oscMessage, const bool boolean) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = boolean ? OscTypeTagTrue : OscTypeTagFalse;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a nil argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddNil(&oscMessage);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddNil(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagNil;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds an infinitum argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInfinitum(&oscMessage);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddInfinitum(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInfinitum;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a 'begin array' argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddBeginArray(&oscMessage);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddBeginArray(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagBeginArray;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Adds a 'end array' argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddEndArray(&oscMessage);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageAddEndArray(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return OscErrorTooManyArguments; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagEndArray;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return OscErrorNone;
}

/**
 * @brief Returns the size (number of bytes) of an OSC message.
 *
 * An example use of this function would be to check whether the OSC message size
 * exceeds the remaining capacity of a containing OSC bundle.
 *
 * Example use:
 * @code
 * if(OscMessageGetSize(&oscMessage) > OscBundleGetRemainingCapacity(&oscBundle)) {
 *     printf("oscMessage is too large to be contained within oscBundle");
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Size (number of bytes) of the OSC bundle.
 */
size_t OscMessageGetSize(const OscMessage * const oscMessage) {
    size_t messageSize = 0;
    messageSize += oscMessage->oscAddressPatternLength + 1; // include null character
    if ((messageSize % 4) != 0) {
        messageSize += 4 - messageSize % 4; // increase to multiple of 4
    }
    messageSize += oscMessage->oscTypeTagStringLength + 1; // include null character
    if ((messageSize % 4) != 0) {
        messageSize += 4 - messageSize % 4; // increase to multiple of 4
    }
    messageSize += oscMessage->argumentsSize;
    return messageSize;
}

/**
 * @brief Converts an OSC message into a byte array to be contained within an
 * OSC packet or OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscMessage OSC message.
 * @param oscMessageSize OSC message size.
 * @param destination Destination byte array.
 * @param destinationSize Destination size that cannot exceed.
 * @return Error code (0 if successful).
 */
OscError OscMessageToCharArray(const OscMessage * const oscMessage, size_t * const oscMessageSize, char * const destination, const size_t destinationSize) {
    *oscMessageSize = 0; // size will be 0 if function unsuccessful
    size_t destinationIndex = 0;
    unsigned int index;

    // Address pattern
    if (oscMessage->oscAddressPatternLength == 0) {
        return OscErrorUndefinedAddressPattern; // error: address pattern not set
    }
    if (oscMessage->oscAddressPattern[0] != '/') {
        return OscErrorNoSlashAtStartOfMessage; // error: address pattern does not start with '/'
    }
    if (oscMessage->oscAddressPatternLength > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    for (index = 0; index < oscMessage->oscAddressPatternLength; index++) {
        destination[destinationIndex++] = oscMessage->oscAddressPattern[index];
    }
    if (TerminateOscString(destination, &destinationIndex, destinationSize) != 0) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }

    // Type tag string
    if ((destinationIndex + oscMessage->oscTypeTagStringLength) > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    for (index = 0; index < oscMessage->oscTypeTagStringLength; index++) {
        destination[destinationIndex++] = oscMessage->oscTypeTagString[index];
    }
    if (TerminateOscString(destination, &destinationIndex, destinationSize) != 0) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }

    // Arguments
    if ((destinationIndex + oscMessage->argumentsSize) > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    for (index = 0; index < oscMessage->argumentsSize; index++) {
        destination[destinationIndex++] = oscMessage->arguments[index];
    }

    *oscMessageSize = destinationIndex;
    return OscErrorNone;
}

/**
 * @brief Terminates an OSC string with one or more null characters so that the
 * OSC string size is a multiple of 4.
 *
 * This is an internal function and cannot be called by the user application.
 *
 * @param oscString OSC string to the terminated.
 * @param oscStringSize Size of the OSC string.
 * @param maxOscStringSize Maximum size of the OSC string that cannot be
 * exceeded.
 * @return 0 if successful.
 */
static int TerminateOscString(char * const oscString, size_t * const oscStringSize, const size_t maxOscStringSize) {
    do {
        if (*oscStringSize >= maxOscStringSize) {
            return 1; // error: string exceeds maximum size
        }
        oscString[(*oscStringSize)++] = '\0';
    } while (*oscStringSize % 4 != 0);
    return 0;
}

//------------------------------------------------------------------------------
// Functions - Message deconstruction

/**
 * @brief Initialises an OSC message from a byte array contained within an OSC
 * packet or OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscMessage OSC message.
 * @param source Byte array.
 * @param numberOfBytes Number of bytes within the byte array.
 * @return Error code (0 if successful).
 */
OscError OscMessageInitialiseFromCharArray(OscMessage * const oscMessage, const char * const source, const size_t numberOfBytes) {
    OscMessageInitialise(oscMessage, "");

    // Return error if not valid OSC message
    if ((numberOfBytes % 4) != 0) {
        return OscErrorSizeIsNotMultipleOfFour; // error: size not multiple of 4
    }
    if (numberOfBytes < MIN_OSC_MESSAGE_SIZE) {
        return OscErrorMessageSizeTooSmall; // error: too few bytes to contain an OSC message
    }
    if (numberOfBytes > MAX_OSC_MESSAGE_SIZE) {
        return OscErrorMessageSizeTooLarge; // error: size exceeds maximum OSC message size
    }
    if (source[0] != '/') {
        return OscErrorNoSlashAtStartOfMessage; // error: first byte is not '/'
    }

    // OSC address pattern
    unsigned int sourceIndex = 0;
    while (source[sourceIndex] != '\0') {
        oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = source[sourceIndex];
        if (++oscMessage->oscAddressPatternLength > MAX_OSC_ADDRESS_PATTERN_LENGTH) {
            return OscErrorAddressPatternTooLong; // error: OSC address pattern too long
        }
        if (++sourceIndex >= numberOfBytes) {
            return OscErrorSourceEndsBeforeEndOfAddressPattern; // error: unexpected end of source
        }
    }
    oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = '\0'; // null terminate string

    // Advance index to OSC type tag string
    while (source[sourceIndex - 1] != ',') { // skip index past comma
        if (++sourceIndex >= numberOfBytes) {
            return OscErrorSourceEndsBeforeStartOfTypeTagString; // error: unexpected end of source
        }
    }

    // OSC type tag string
    while (source[sourceIndex] != '\0') {
        oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = source[sourceIndex];
        if (++oscMessage->oscTypeTagStringLength > MAX_OSC_TYPE_TAG_STRING_LENGTH) {
            return OscErrorTypeTagStringToLong; // error: type tag string too long
        }
        if (++sourceIndex >= numberOfBytes) {
            return OscErrorSourceEndsBeforeEndOfTypeTagString; // error: unexpected end of source
        }
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string

    // Advance index to arguments
    do {
        if (++sourceIndex > numberOfBytes) {
            return OscErrorUnexpectedEndOfSource; // error: unexpected end of source
        }
    } while (sourceIndex % 4 != 0);

    // Arguments
    while (sourceIndex < numberOfBytes) {
        oscMessage->arguments[oscMessage->argumentsSize++] = source[sourceIndex++];
    }

    return OscErrorNone;
}

/**
 * @brief Returns true if an argument is available indicated by the current
 * oscTypeTagStringIndex value.
 *
 * Example use:
 * @code
 * if(OscMessageIsArgumentAvailable(&oscMessage)) {
 *     printf("Argument is available");
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return True if an argument is available.
 */
bool OscMessageIsArgumentAvailable(OscMessage * const oscMessage) {
    return oscMessage->oscTypeTagStringIndex <= oscMessage->oscTypeTagStringLength - 1;
}

/**
 * @brief Returns OSC type tag of the next argument available within an OSC
 * message indicated by the current oscTypeTagStringIndex value.
 *
 * A null character (value zero) will be returned if no arguments are available.
 *
 * Example use:
 * @code
 * const OscTypeTag oscTypeTag = OscMessageGetArgumentType(&oscMessage);
 * printf("The next argument is: %c", (char)oscTypeTag);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Next type tag in type tag string.
 */
OscTypeTag OscMessageGetArgumentType(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringIndex > oscMessage->oscTypeTagStringLength) {
        return '\0'; // error: end of type tag string
    }
    return (OscTypeTag) oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex];
}

/**
 * @brief Skips the next argument available within an OSC message indicated by
 * the current oscTypeTagStringIndex value.
 *
 * Example use:
 * @code
 * while(true) { // loop to skip to first int32 argument
 *     if(OscMessageGetArgumentType(&oscMessage) == OscTypeTagInt32) {
 *         break; // found int32 argument
 *     }
 *     if(OscMessageSkipArgument(&oscMessage)) {
 *         break; // error: no more arguments available
 *     }
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @return Error code (0 if successful).
 */
OscError OscMessageSkipArgument(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 32-bit integer argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 32-bit integer else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagInt32:
 *     {
 *         int32_t int32;
 *         OscMessageGetInt32(&oscMessage, &int32);
 *         printf("Value = %d", int32);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int32 32-bit integer argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetInt32(OscMessage * const oscMessage, int32_t * const int32) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagInt32) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *int32 = oscArgument32.int32;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 32-bit float argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 32-bit float else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagFloat32:
 *     {
 *         float float32;
 *         OscMessageGetFloat32(&oscMessage, &float32);
 *         printf("Value = %f", float32);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param float32 32-bit float argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetFloat32(OscMessage * const oscMessage, float * const float32) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagFloat32) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *float32 = oscArgument32.float32;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a string or alternate string argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a string else this function will return
 * an error.  The internal index oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagString:
 *     {
 *         char string[128];
 *         OscMessageGetString(&oscMessage, string, sizeof(string));
 *         printf("Value = %s", string);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param destination String or alternate string argument
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetString(OscMessage * const oscMessage, char * const destination, const size_t destinationSize) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if ((oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagString)
            && (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagAlternateString)) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof ("\0\0\0")) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    if (sizeof ("\0\0\0") > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    unsigned int argumentsIndex = oscMessage->argumentsIndex; // local copy in case function returns error
    unsigned int destinationIndex = 0;
    do {
        destination[destinationIndex] = oscMessage->arguments[argumentsIndex];
        if (++destinationIndex > destinationSize) {
            return OscErrorDestinationTooSmall; // error: destination too small
        }
    } while (oscMessage->arguments[argumentsIndex++] != '\0');
    while ((argumentsIndex % 4) != 0) {
        if (++argumentsIndex > oscMessage->argumentsSize) {
            return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
        }
    }
    oscMessage->argumentsIndex = argumentsIndex;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a blob (byte array) argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a blob else this function will return
 * an error.  The internal index oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagBlob:
 *     {
 *         char byteArray[128];
 *         size_t numberOfBytes;
 *         OscMessageGetBlob(&oscMessage, &numberOfBytes, byteArray, sizeof(byteArray));
 *         unsigned int index = 0;
 *         while(index <= numberOfBytes) {
 *             printf("%u,", (unsigned int)byteArray[index]);
 *         }
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param blobSize Blob argument size (number of bytes).
 * @param destination Blob argument (byte array).
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetBlob(OscMessage * const oscMessage, size_t * const blobSize, char * const destination, const size_t destinationSize) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagBlob) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    unsigned int argumentsIndex = oscMessage->argumentsIndex; // local copy in case function returns error
    OscArgument32 blobSizeArgument;
    blobSizeArgument.byteStruct.byte3 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte2 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte1 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte0 = oscMessage->arguments[argumentsIndex++];
    if ((argumentsIndex + blobSizeArgument.int32) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    if (blobSizeArgument.int32 > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    unsigned int destinationIndex;
    for (destinationIndex = 0; destinationIndex < blobSizeArgument.int32; destinationIndex++) {
        destination[destinationIndex] = oscMessage->arguments[argumentsIndex++];
    }
    while ((argumentsIndex % 4) != 0) {
        if (++argumentsIndex > oscMessage->argumentsSize) {
            return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
        }
    }
    oscMessage->argumentsIndex = argumentsIndex;
    *blobSize = blobSizeArgument.int32;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 64-bit integer argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 64-bit integer else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagInt64:
 *     {
 *         int64_t int64;
 *         OscMessageGetInt64(&oscMessage, &int64);
 *         printf("Value = %d", int64);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int64 64-bit integer argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetInt64(OscMessage * const oscMessage, int64_t * const int64) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagInt64) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument64)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument64 oscArgument64;
    oscArgument64.byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *int64 = oscArgument64.int64;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets an OSC time tag argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be an OSC time tag else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagTimeTag:
 *     {
 *         OscTimeTag oscTimeTag;
 *         OscMessageGetTimeTag(&oscMessage, &oscTimeTag);
 *         printf("Value = %u", (unsigned int)oscTimeTag.dwordStruct.seconds);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param oscTimeTag OSC time tag argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetTimeTag(OscMessage * const oscMessage, OscTimeTag * const oscTimeTag) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagTimeTag) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscTimeTag)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    oscTimeTag->byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 64-bit double argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 64-bit double else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagDouble:
 *     {
 *         double double64;
 *         OscMessageGetDouble(&oscMessage, &double64);
 *         printf("Value = %f", double64);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param double64 64-bit double argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetDouble(OscMessage * const oscMessage, Double64 * const double64) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagDouble) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument64)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument64 oscArgument64;
    oscArgument64.byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *double64 = oscArgument64.double64;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a character argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a character else this function will
 * return an error.  The internal index oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagCharacter:
 *     {
 *         char character;
 *         OscMessageGetCharacter(&oscMessage, &character);
 *         printf("Value = %c", character);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param character Character argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetCharacter(OscMessage * const oscMessage, char * const character) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagCharacter) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    oscMessage->argumentsIndex += 3;
    *character = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 32-bit RGBA colour argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 32-bit RGBA colour else this function
 * will return an error.  The internal index oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagRgbaColour:
 *     {
 *         RgbaColour rgbaColour;
 *         OscMessageGetRgbaColour(&oscMessage, &rgbaColour);
 *         printf("Value = %u,%u,%u,%u", rgbaColour.red, rgbaColour.green, rgbaColour.blue, rgbaColour.alpha);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param rgbaColour 32-bit RGBA colour argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetRgbaColour(OscMessage * const oscMessage, RgbaColour * const rgbaColour) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagRgbaColour) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *rgbaColour = oscArgument32.rgbaColour;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Gets a 4 byte MIDI message argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index oscTypeTagStringIndex) must be a 4 byte MIDI message else this
 * function will return an error.  The internal index oscTypeTagStringIndex,
 * will only be incremented to the next argument if this function is successful.
 * The user application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagMidiMessage:
 *     {
 *         MidiMessage midiMessage;
 *         OscMessageGetMidiMessage(&oscMessage, &midiMessage);
 *         printf("Value = %u,%u,%u,%u", midiMessage.portID, midiMessage.status, midiMessage.data1, midiMessage.data2);
 *         break;
 *     }
 *     default:
 *         printf("Expected argument not available");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param midiMessage 4 byte MIDI message argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetMidiMessage(OscMessage * const oscMessage, MidiMessage * const midiMessage) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] == '\0') {
        return OscErrorNoArgumentsAvailable; // error: end of type tag string
    }
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagMidiMessage) {
        return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    if ((oscMessage->argumentsIndex + sizeof (OscArgument32)) > oscMessage->argumentsSize) {
        return OscErrorMessageTooShortForArgumentType; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *midiMessage = oscArgument32.midiMessage;
    oscMessage->oscTypeTagStringIndex++;
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as an int32 even if
 * the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * int32_t int32;
 * OscMessageGetArgumentAsInt32(&oscMessage, &int32);
 * printf("Value = %d", int32);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int32 32-bit integer argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsInt32(OscMessage * const oscMessage, int32_t * const int32) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            return OscMessageGetInt32(oscMessage, int32);
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            *int32 = (int32_t) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            *int32 = (int32_t) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *int32 = (int32_t) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            *int32 = (int32_t) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            *int32 = (int32_t) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            *int32 = (int32_t) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *int32 = (int32_t) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *int32 = 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *int32 = UINT32_MAX;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as an float32 even if
 * the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * float float32;
 * OscMessageGetArgumentAsFloat32(&oscMessage, &float32);
 * printf("Value = %f", float32);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param float32 32-bit float argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsFloat32(OscMessage * const oscMessage, float * const float32) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            *float32 = (float) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            return OscMessageGetFloat32(oscMessage, float32);
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            *float32 = (float) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *float32 = (float) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            *float32 = (float) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            *float32 = (float) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            *float32 = (float) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *float32 = (float) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *float32 = 0.0f;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *float32 = 1.0f / 0.0f;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a string even if
 * the argument is of another type.
 *
 * The argument provided must be either a string, blob, alternate string, or
 * character.  The internal index oscTypeTagStringIndex will only be incremented
 * to the next argument if this function is successful.
 *
 * Example use:
 * @code
 * char string[128];
 * OscMessageGetArgumentAsString(&oscMessage, string, sizeof(string));
 * printf("Value = %s", string);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param destination String argument.
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsString(OscMessage * const oscMessage, char * const destination, const size_t destinationSize) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagString:
        {
            return OscMessageGetString(oscMessage, destination, destinationSize);
        }
        case OscTypeTagBlob:
        {
            size_t blobSize;
            const OscError oscError = OscMessageGetBlob(oscMessage, &blobSize, destination, destinationSize);
            if (oscError != 0) {
                return oscError;
            }
            if (destination[blobSize - 1] != '\0') { // if blob not null terminated
                if (blobSize >= destinationSize) {
                    return OscErrorDestinationTooSmall; // error: destination too small
                }
                destination[blobSize] = '\0';
            }
            return OscErrorNone;
        }
        case OscTypeTagAlternateString:
        {
            return OscMessageGetString(oscMessage, destination, destinationSize);
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            if (oscError != 0) {
                return oscError;
            }
            if (destinationSize < 2) {
                return OscErrorDestinationTooSmall; // error: destination too small
            }
            destination[0] = character;
            destination[1] = '\0'; // null terminate string
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a blob even if
 * the argument is of another type.
 *
 * The argument provided must be either a string, blob, alternate string, or
 * character.  The internal index oscTypeTagStringIndex will only be incremented
 * to the next argument if this function is successful.
 *
 * Example use:
 * @code
 * char byteArray[128];
 * size_t numberOfBytes;
 * OscMessageGetArgumentAsBlob(&oscMessage, &numberOfBytes, byteArray, sizeof(byteArray));
 * unsigned int index = 0;
 * while(index <= numberOfBytes) {
 *     printf("%u,", (unsigned int)byteArray[index]);
 * }
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param blobSize Blob argument size (number of bytes).
 * @param destination Blob argument (byte array).
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsBlob(OscMessage * const oscMessage, size_t * const blobSize, char * const destination, const size_t destinationSize) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagString:
        case OscTypeTagAlternateString:
        {
            const OscError oscError = OscMessageGetString(oscMessage, destination, destinationSize);
            if (oscError != 0) {
                return oscError;
            }
            *blobSize = strlen(destination);
            return OscErrorNone;
        }
        case OscTypeTagBlob:
        {
            return OscMessageGetBlob(oscMessage, blobSize, destination, destinationSize);
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            if (oscError != 0) {
                return oscError;
            }
            if (destinationSize < 1) {
                return OscErrorDestinationTooSmall;
            }
            destination[0] = character;
            *blobSize = 1;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as an int64 even if
 * the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * int64_t int64;
 * OscMessageGetArgumentAsInt64(&oscMessage, &int64);
 * printf("Value = %d", int64);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param int64 64-bit integer argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsInt64(OscMessage * const oscMessage, int64_t * const int64) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            *int64 = (int64_t) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            *int64 = (int64_t) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            return OscMessageGetInt64(oscMessage, int64);
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *int64 = (int64_t) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            *int64 = (int64_t) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            *int64 = (int64_t) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            *int64 = (int64_t) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *int64 = (int64_t) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *int64 = (int64_t) 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *int64 = INT64_MAX;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as an OSC Time Tag
 * even if the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * OscTimeTag oscTimeTag;
 * OscMessageGetArgumentAsTimeTag(&oscMessage, &oscTimeTag);
 * printf("Value = %u", (unsigned int)oscTimeTag.dwordStruct.seconds);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param oscTimeTag OSC time tag argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsTimeTag(OscMessage * const oscMessage, OscTimeTag * const oscTimeTag) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            oscTimeTag->value = (uint64_t) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            oscTimeTag->value = (uint64_t) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            oscTimeTag->value = (uint64_t) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            return OscMessageGetTimeTag(oscMessage, oscTimeTag);
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            oscTimeTag->value = (uint64_t) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            oscTimeTag->value = (uint64_t) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            oscTimeTag->value = (uint64_t) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            oscTimeTag->value = (uint64_t) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            oscTimeTag->value = (uint64_t) 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            oscTimeTag->value = (uint64_t) INT64_MAX;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a 64-bit double
 * even if the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * double double64;
 * OscMessageGetArgumentAsDouble(&oscMessage, &double64);
 * printf("Value = %f", double64);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param double64 64-bit double argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsDouble(OscMessage * const oscMessage, Double64 * const double64) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            *double64 = (Double64) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            *double64 = (Double64) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            *double64 = (Double64) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *double64 = (Double64) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            return OscMessageGetDouble(oscMessage, double64);
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            *double64 = (Double64) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            *double64 = (Double64) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *double64 = (Double64) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *double64 = (Double64) 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *double64 = (Double64) 1 / (Double64) 0;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a character even if
 * the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * char character;
 * OscMessageGetArgumentAsCharacter(&oscMessage, &character);
 * printf("Value = %c", character);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param character Character argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsCharacter(OscMessage * const oscMessage, char * const character) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            *character = (char) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            *character = (char) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            *character = (char) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *character = (char) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            *character = (char) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            return OscMessageGetCharacter(oscMessage, character);
        }
        case OscTypeTagTrue:
        {
            *character = (char) true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *character = (char) false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *character = (char) 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *character = CHAR_MAX;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a 32-bit RGBA
 * colour even if the argument is of another type.
 *
 * The argument provided must be either a blob or 32-bit RGBA colour.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * RgbaColour rgbaColour;
 * OscMessageGetArgumentAsRgbaColour(&oscMessage, &rgbaColour);
 * printf("Value = %u,%u,%u,%u", rgbaColour.red, rgbaColour.green, rgbaColour.blue, rgbaColour.alpha);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param rgbaColour 32-bit RGBA colour argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsRgbaColour(OscMessage * const oscMessage, RgbaColour * const rgbaColour) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagBlob:
        {
            size_t blobSize;
            const OscError oscError = OscMessageGetBlob(oscMessage, &blobSize, (char *) &rgbaColour, sizeof (RgbaColour));
            if (oscError != 0) {
                return oscError;
            }
            if (blobSize != sizeof (RgbaColour)) {
                return OscErrorUnexpectedEndOfSource; // error: not enough bytes in blob
            }
            return OscErrorNone;
        }
        case OscTypeTagRgbaColour:
        {
            return OscMessageGetRgbaColour(oscMessage, rgbaColour);
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a 4 byte MIDI
 * message even if the argument is of another type.
 *
 * The argument provided must be either a blob or 4 byte MIDI message.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * MidiMessage midiMessage;
 * OscMessageGetArgumentAsMidiMessage(&oscMessage, &midiMessage);
 * printf("Value = %u,%u,%u,%u", midiMessage.portID, midiMessage.status, midiMessage.data1, midiMessage.data2);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param midiMessage 4 byte MIDI message argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsMidiMessage(OscMessage * const oscMessage, MidiMessage * const midiMessage) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagBlob:
        {
            size_t blobSize;
            const OscError oscError = OscMessageGetBlob(oscMessage, &blobSize, (char *) &midiMessage, sizeof (MidiMessage));
            if (oscError != 0) {
                return oscError;
            }
            if (blobSize != sizeof (MidiMessage)) {
                return OscErrorUnexpectedEndOfSource; // error: not enough bytes in blob
            }
            return OscErrorNone;
        }
        case OscTypeTagMidiMessage:
        {
            return OscMessageGetMidiMessage(oscMessage, midiMessage);
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

/**
 * @brief Interprets the next argument in the OSC message as a boolean even if
 * the argument is of another type.
 *
 * The argument provided must be of a numerical type: int32, float32, int64,
 * OSC time tag, 64-bit double, character, boolean, nil, or infinitum.  The
 * internal index oscTypeTagStringIndex will only be incremented to the next
 * argument if this function is successful.
 *
 * Example use:
 * @code
 * bool boolean;
 * OscMessageGetArgumentAsBool(&oscMessage, &boolean);
 * printf("Value = %u", boolean);
 * @endcode
 *
 * @param oscMessage OSC message.
 * @param boolean Boolean argument.
 * @return Error code (0 if successful).
 */
OscError OscMessageGetArgumentAsBool(OscMessage * const oscMessage, bool * const boolean) {
    if (OscMessageIsArgumentAvailable(oscMessage) == false) {
        return OscErrorNoArgumentsAvailable;
    }
    switch (OscMessageGetArgumentType(oscMessage)) {
        case OscTypeTagInt32:
        {
            int32_t int32;
            const OscError oscError = OscMessageGetInt32(oscMessage, &int32);
            *boolean = (bool) int32;
            return oscError;
        }
        case OscTypeTagFloat32:
        {
            float float32;
            const OscError oscError = OscMessageGetFloat32(oscMessage, &float32);
            *boolean = (bool) float32;
            return oscError;
        }
        case OscTypeTagInt64:
        {
            int64_t int64;
            const OscError oscError = OscMessageGetInt64(oscMessage, &int64);
            *boolean = (bool) int64;
            return oscError;
        }
        case OscTypeTagTimeTag:
        {
            OscTimeTag oscTimeTag;
            const OscError oscError = OscMessageGetTimeTag(oscMessage, &oscTimeTag);
            *boolean = (bool) oscTimeTag.value;
            return oscError;
        }
        case OscTypeTagDouble:
        {
            Double64 double64;
            const OscError oscError = OscMessageGetDouble(oscMessage, &double64);
            *boolean = (bool) double64;
            return oscError;
        }
        case OscTypeTagCharacter:
        {
            char character;
            const OscError oscError = OscMessageGetCharacter(oscMessage, &character);
            *boolean = (bool) character;
            return oscError;
        }
        case OscTypeTagTrue:
        {
            *boolean = true;
            return OscErrorNone;
        }
        case OscTypeTagFalse:
        {
            *boolean = false;
            return OscErrorNone;
        }
        case OscTypeTagNil:
        {
            *boolean = (bool) 0;
            return OscErrorNone;
        }
        case OscTypeTagInfinitum:
        {
            *boolean = true;
            return OscErrorNone;
        }
        default:
            return OscErrorUnexpectedArgumentType; // error: unexpected argument type
    }
    return OscErrorNone;
}

//------------------------------------------------------------------------------
// End of file
