#pragma once
#include <midici\midi2_MidiCiMessages.h>

namespace midi2 { namespace midici {

class MessageInterpreter
{
public:
    UniversalSysExMessageBase ReadMessage(char* buffer, size_t bufferSize);
};

}}