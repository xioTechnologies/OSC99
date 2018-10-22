/**
 * @file OscBundle.c
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * bundles.
 * See http://opensoundcontrol.org/spec-1_0
 */

//------------------------------------------------------------------------------
// Includes

#include "OscBundle.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises an OSC bundle with a specified OSC time tag.
 *
 * An OSC bundle must be initialised before use.  The oscTimeTag argument may be
 * specified as oscTimeTagZero for an OSC time tag value of  zero. This may be
 * of use if the OSC time tag value is irrelevant to the user application, if
 * the contained OSC messages should be invoke immediately, or if the OSC time
 * tag value is intended to be overwritten after initialisation of the OSC
 * bundle.
 *
 * Example use:
 * @code
 * OscBundle oscBundle;
 * OscBundleInitialise(&oscBundle, oscTimeTagZero);
 * oscBundle.oscTimeTag.value = 0x100000000; // overwrite OSC time tag with value of 1 second
 * @endcode
 *
 * @param oscBundle OSC bundle to be initialised.
 * @param oscTimeTag OSC time tag.
 */
void OscBundleInitialise(OscBundle * const oscBundle, const OscTimeTag oscTimeTag) {
    oscBundle->header[0] = OSC_BUNDLE_HEADER[0];
    oscBundle->header[1] = OSC_BUNDLE_HEADER[1];
    oscBundle->header[2] = OSC_BUNDLE_HEADER[2];
    oscBundle->header[3] = OSC_BUNDLE_HEADER[3];
    oscBundle->header[4] = OSC_BUNDLE_HEADER[4];
    oscBundle->header[5] = OSC_BUNDLE_HEADER[5];
    oscBundle->header[6] = OSC_BUNDLE_HEADER[6];
    oscBundle->header[7] = OSC_BUNDLE_HEADER[7];
    oscBundle->oscTimeTag = oscTimeTag;
    oscBundle->oscBundleElementsSize = 0;
}

/**
 * @brief Adds an OSC message or OSC bundle to an OSC bundle.
 *
 * The oscContents argument must point to an initialised OSC message or OSC
 * bundle.  This function may be called multiple times to add multiple  OSC
 * messages or OSC bundles to a containing OSC bundle.  If the remaining
 * capacity of the containing OSC bundle is insufficient to hold the additional
 * contents then the additional contents will be discarded and the function will
 * return an error.
 *
 * Example use:
 * @code
 * OscMessage oscMessageToAdd;
 * OscMessageInitialise(&oscMessageToAdd, "/example/address/pattern");
 *
 * OscBundle oscBundleToAdd;
 * OscBundleInitialise(&oscBundleToAdd, oscTimeTagZero);
 *
 * OscBundle oscBundle;
 * OscBundleInitialise(&oscBundle, oscTimeTagZero);
 * OscBundleAddContents(&oscBundle, &oscMessageToAdd);
 * OscBundleAddContents(&oscBundle, &oscBundleToAdd);
 * @endcode
 *
 * @param oscBundle OSC bundle that will contain the OSC message or OSC bundle
 * to be added.
 * @param oscContents OSC message or OSC bundle to be added to the OSC bundle.
 * @return Error code (0 if successful).
 */
OscError OscBundleAddContents(OscBundle * const oscBundle, const void * const oscContents) {
    if ((oscBundle->oscBundleElementsSize + sizeof (OscArgument32)) > MAX_OSC_BUNDLE_ELEMENTS_SIZE) {
        return OscErrorBundleFull; // error: bundle full
    }
    OscBundleElement oscBundleElement;
    oscBundleElement.contents = &oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize + sizeof (OscArgument32)];
    OscError oscError = OscErrorInvalidContents; // error: invalid or uninitialised OSC contents
    if (OscContentsIsMessage(oscContents) == true) {
        size_t oscBundleElementSize;
        oscError = OscMessageToCharArray((OscMessage *) oscContents, &oscBundleElementSize, oscBundleElement.contents, OscBundleGetRemainingCapacity(oscBundle));
        oscBundleElement.size.int32 = (int32_t) oscBundleElementSize;
    }
    if (OscContentsIsBundle(oscContents) == true) {
        size_t oscBundleElementSize;
        oscError = OscBundleToCharArray((OscBundle *) oscContents, &oscBundleElementSize, oscBundleElement.contents, OscBundleGetRemainingCapacity(oscBundle));
        oscBundleElement.size.int32 = (int32_t) oscBundleElementSize;
    }
    if (oscError != 0) {
        return oscError; // error: ???
    }
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte3;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte2;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte1;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte0;
    oscBundle->oscBundleElementsSize += oscBundleElement.size.int32;
    return OscErrorNone;
}

/**
 * @brief Empties an OSC bundle.
 *
 * All OSC bundle elements contained within the OSC bundle are discarded.  The
 * OSC bundle's OSC time tag is not modified.
 *
 * Example use:
 * @code
 * OscBundleEmpty(&oscBundle);
 * @endcode
 *
 * @param oscBundle OSC bundle to be emptied.
 */
void OscBundleEmpty(OscBundle * const oscBundle) {
    oscBundle->oscBundleElementsSize = 0;
}

/**
 * @brief Returns true if the OSC bundle is empty.
 *
 * An empty OSC bundle contains no OSC bundle elements (OSC messages or OSC
 * bundles) but does retain an OSC time tag.
 *
 * Example use:
 * @code
 * if(OscBundleIsEmpty(&oscBundle)) {
 *     printf("oscBundle is empty.");
 * }
 * @endcode
 *
 * @param oscBundle OSC bundle.
 * @return True if the OSC bundle is empty.
 */
bool OscBundleIsEmpty(OscBundle * const oscBundle) {
    return oscBundle->oscBundleElementsSize == 0;
}

/**
 * @brief Returns the remaining capacity (number of bytes) of an OSC bundle.
 *
 * The remaining capacity of an OSC bundle is the number of bytes available to
 * contain an OSC message or OSC bundle.
 *
 * Example use:
 * @code
 * const size_t remainingCapacity = OscBundleGetRemainingCapacity(&oscBundle);
 * @endcode
 *
 * @param oscBundle OSC bundle.
 * @return Remaining capacity (number of bytes) of an OSC bundle.
 */
size_t OscBundleGetRemainingCapacity(const OscBundle * const oscBundle) {
    const size_t remainingCapacity = MAX_OSC_BUNDLE_ELEMENTS_SIZE - oscBundle->oscBundleElementsSize - sizeof (OscArgument32); // account for int32 size required by OSC bundle element
    if ((int) remainingCapacity < 0) {
        return 0; // avoid negative result of capacity calculation
    }
    return remainingCapacity;
}

/**
 * @brief Returns the size (number of bytes) of an OSC bundle.
 *
 * An example use of this function would be to check whether the OSC bundle size
 * exceeds the remaining capacity of a containing OSC bundle.
 *
 * Example use:
 * @code
 * if(OscBundleGetSize(&oscBundleChild) > OscBundleGetRemainingCapacity(&oscBundleParent)) {
 *     printf("oscBundleChild is too large to be contained within oscBundleParent");
 * }
 * @endcode
 *
 * @param oscBundle OSC bundle.
 * @return Size (number of bytes) of the OSC bundle.
 */
size_t OscBundleGetSize(const OscBundle * const oscBundle) {
    return sizeof (OSC_BUNDLE_HEADER) + sizeof (OscTimeTag) + oscBundle->oscBundleElementsSize;
}

/**
 * @brief Converts an OSC bundle into a byte array to be contained within an OSC
 * packet or containing OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle OSC bundle.
 * @param oscBundleSize OSC bundle size.
 * @param destination Destination byte array.
 * @param destinationSize Destination size that cannot exceed.
 * @return Error code (0 if successful).
 */
OscError OscBundleToCharArray(const OscBundle * const oscBundle, size_t * const oscBundleSize, char * const destination, const size_t destinationSize) {
    *oscBundleSize = 0; // size will be 0 if function unsuccessful
    if ((sizeof (OSC_BUNDLE_HEADER) + sizeof (OscTimeTag) + oscBundle->oscBundleElementsSize) > destinationSize) {
        return OscErrorDestinationTooSmall; // error: destination too small
    }
    size_t destinationIndex = 0;
    unsigned int index;
    for (index = 0; index < sizeof (OSC_BUNDLE_HEADER); index++) {
        destination[destinationIndex++] = oscBundle->header[index];
    }
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte7;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte6;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte5;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte4;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte3;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte2;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte1;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte0;
    for (index = 0; index < oscBundle->oscBundleElementsSize; index++) {
        destination[destinationIndex++] = oscBundle->oscBundleElements[index];
    }
    *oscBundleSize = destinationIndex;
    return OscErrorNone;
}

/**
 * @brief Initialises an OSC bundle from a byte array contained within an OSC
 * packet or containing OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle OSC bundle.
 * @param source Byte array.
 * @param numberOfBytes Number of bytes in byte array.
 * @return Error code (0 if successful).
 */
OscError OscBundleInitialiseFromCharArray(OscBundle * const oscBundle, const char * const source, const size_t numberOfBytes) {
    unsigned int sourceIndex = 0;

    // Return error if not valid bundle
    if (numberOfBytes % 4 != 0) {
        return OscErrorSizeIsNotMultipleOfFour; // error: size not multiple of 4
    }
    if (numberOfBytes < MIN_OSC_BUNDLE_SIZE) {
        return OscErrorBundleSizeTooSmall; // error: too few bytes to contain bundle
    }
    if (numberOfBytes > MAX_OSC_BUNDLE_SIZE) {
        return OscErrorBundleSizeTooLarge; // error: size exceeds maximum bundle size
    }
    if (source[sourceIndex] != '#') {
        return OscErrorNoHashAtStartOfBundle; // error: first byte is not '#'
    }

    // Header
    oscBundle->header[0] = source[sourceIndex++];
    oscBundle->header[1] = source[sourceIndex++];
    oscBundle->header[2] = source[sourceIndex++];
    oscBundle->header[3] = source[sourceIndex++];
    oscBundle->header[4] = source[sourceIndex++];
    oscBundle->header[5] = source[sourceIndex++];
    oscBundle->header[6] = source[sourceIndex++];
    oscBundle->header[7] = source[sourceIndex++];

    // OSC time tag
    oscBundle->oscTimeTag.byteStruct.byte7 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte6 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte5 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte4 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte3 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte2 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte1 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte0 = source[sourceIndex++];

    // Osc bundle elements
    oscBundle->oscBundleElementsSize = 0;
    oscBundle->oscBundleElementsIndex = 0;
    do {
        oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = source[sourceIndex++];
    } while (sourceIndex < numberOfBytes);

    return OscErrorNone;
}

/**
 * @brief Returns true if an OSC bundle element is available based on the
 * current oscBundleElementsIndex value.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle OSC bundle.
 * @return True if a bundle element is available.
 */
bool OscBundleIsBundleElementAvailable(const OscBundle * const oscBundle) {
    return (oscBundle->oscBundleElementsIndex + sizeof (OscArgument32)) < oscBundle->oscBundleElementsSize;
}

/**
 * @brief Gets the next OSC bundle element available within the OSC bundle based
 * on the current oscBundleElementsIndex.
 *
 * oscBundleElementsIndex will be incremented to the next OSC bundle element if
 * this function is successful.  Otherwise, the oscBundleElementsIndex will
 * remain unmodified.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle OSC bundle.
 * @param oscBundleElement OSC bundle element.
 * @return Error code (0 if successful).
 */
OscError OscBundleGetBundleElement(OscBundle * const oscBundle, OscBundleElement * const oscBundleElement) {
    if ((oscBundle->oscBundleElementsIndex + sizeof (OscArgument32)) >= oscBundle->oscBundleElementsSize) {
        return OscErrorBundleElementNotAvailable; // error: too few bytes to contain bundle element
    }
    oscBundleElement->size.byteStruct.byte3 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte2 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte1 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte0 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    if (oscBundleElement->size.int32 < 0) {
        return OscErrorNegativeBundleElementSize; // error: size cannot be negative
    }
    if ((oscBundleElement->size.int32 % 4) != 0) {
        return OscErrorSizeIsNotMultipleOfFour; // error: size not multiple of 4
    }
    if ((oscBundle->oscBundleElementsIndex + oscBundleElement->size.int32) > oscBundle->oscBundleElementsSize) {
        return OscErrorInvalidElementSize; // error: too few bytes for indicated size
    }
    oscBundleElement->contents = &oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex];
    oscBundle->oscBundleElementsIndex += oscBundleElement->size.int32;
    return OscErrorNone;
}

//------------------------------------------------------------------------------
// End of file
