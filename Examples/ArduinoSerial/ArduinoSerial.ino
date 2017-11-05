/**
 * @file ArduinoSerial.ino
 * @author Seb Madgwick
 * @brief Simple OSC99 example for the Arduino Uno.
 *
 * THE FOLLOWING TWO ACTIONS MUST BE TAKEN TO USE THIS EXAMPLE:
 *
 * 1) The OSC99 source files (i.e. the "Osc99" directory) must be added to the
 * Arduino libraries folder.  See: https://www.arduino.cc/en/guide/libraries
 *
 * 2) The Arduino Uno only has a very small amount of RAM (2 kB).  The value of
 * MAX_TRANSPORT_SIZE must be reduced to 200 in OscCommon.h.
 */

#include "Osc99.h"

OscSlipDecoder oscSlipDecoder;

void setup() {
  Serial.begin(115200);
  OscSlipDecoderInitialise(&oscSlipDecoder);
  oscSlipDecoder.processPacket = ProcessPacket; // assign callback function
}

void loop() {

  // Send analogue inputs every 100 ms (10 Hz)
  static unsigned long previousMillis; // static means that the value will be remembered each loop
  unsigned long currentMillis = millis();
  if((currentMillis - previousMillis) > 100) {
    sendAnalogueInputsMessage();
    previousMillis = currentMillis;
  }

  // Process each received byte through the SLIP decoder
  while(Serial.available() > 0) {
    OscSlipDecoderProcessByte(&oscSlipDecoder, Serial.read());
  }
}

// This function is called for each OSC packet received by the SLIP decoder
void ProcessPacket(OscPacket* const oscPacket) {
  oscPacket->processMessage = &ProcessMessage; // assign callback function
  OscPacketProcessMessages(oscPacket);
}

// This function is called for each OSC message found within the received OSC packet
void ProcessMessage(const OscTimeTag* const oscTimeTag, OscMessage* const oscMessage) {

  // If message address is "/hello" then send our hello message
  if (OscAddressMatch(oscMessage->oscAddressPattern, "/hello") == true) {
    sendHelloMessage();
    return;
  }

  // If message address is "/digital/write" then get use the message arguments to set a digital pin
  if (OscAddressMatch(oscMessage->oscAddressPattern, "/digital/write")) {
    int32_t pinNumber, pinState;
    if(OscMessageGetArgumentAsInt32(oscMessage, &pinNumber) != OscErrorNone) {
      return; // error: unable to get first int32 argument
    }
    if(OscMessageGetArgumentAsInt32(oscMessage, &pinState) != OscErrorNone) {
      return; // error: unable to get second int32 argument
    }
    if(pinNumber < 2) {
      return; // error: cannot modify pins being used for serial
    }
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, pinState);
    return;
  }

  // Message address not recognised so send our error message to let the user know
  sendErrorMessage();
}

void sendHelloMessage() {
  OscMessage oscMessage;
  OscMessageInitialise(&oscMessage, "/hello");
  OscMessageAddString(&oscMessage, "Hi!");
  sendOscContents(&oscMessage);
}

void sendErrorMessage() {
  OscMessage oscMessage;
  OscMessageInitialise(&oscMessage, "/error");
  OscMessageAddString(&oscMessage, "OSC address not recognised.");
  sendOscContents(&oscMessage);
}

void sendAnalogueInputsMessage() {
  OscMessage oscMessage;
  OscMessageInitialise(&oscMessage, "/analog/read");
  OscMessageAddInt32(&oscMessage, analogRead(A0)); // read analogue input pin
  OscMessageAddInt32(&oscMessage, analogRead(A1));
  OscMessageAddInt32(&oscMessage, analogRead(A2));
  OscMessageAddInt32(&oscMessage, analogRead(A3));
  sendOscContents(&oscMessage);
}

void sendOscContents(void * oscContents) {

  // Create OSC packet from OSC message or OSC bundle
  OscPacket oscPacket;
  if(OscPacketInitialiseFromContents(&oscPacket, oscContents) != OscErrorNone) {
    return; // error: unable to create an OSC packet from the OSC contents
  }

  // Encode SLIP packet
  char slipPacket[MAX_OSC_PACKET_SIZE];
  size_t slipPacketSize;
  if(OscSlipEncodePacket(&oscPacket, &slipPacketSize, slipPacket, sizeof (slipPacket)) != OscErrorNone) {
    return; // error: the encoded SLIP packet is too long for the size of slipPacket
  }

  // Send SLIP packet
  Serial.write((uint8_t*)slipPacket, slipPacketSize); // typecast from char* to uint8_t*
}
