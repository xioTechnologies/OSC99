# OSC99

OSC99 is a portable ANSI C99 compliant OSC library developed for use with embedded systems.  OSC99 implements the [OSC 1.0 specification](http://opensoundcontrol.org/spec-1_0) including all optional argument types.  The library also includes a [SLIP](https://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol) module for encoding and decoding OSC packets via unframed protocols such as UART/serial as required by the [OSC 1.1 specification](http://opensoundcontrol.org/spec-1_1). 

The following definitions may be modified in OscCommon.h as required by the user application: `LITTLE_ENDIAN_PLATFORM`, `MAX_TRANSPORT_SIZE`, `OSC_ERROR_MESSAGES_ENABLED`.
