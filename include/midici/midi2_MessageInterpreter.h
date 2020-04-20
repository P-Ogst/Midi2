#pragma once
#include <midici\midi2_MidiCiMessages.h>

namespace midi2 { namespace midici {

class MessageInterpreter
{
public:
    UniversalSysExMessageBase ReadMessage(int* pOutReadBytes, void* buffer, size_t bufferSize);
private:
    UniversalSysExMessageBase Parse(int* pOutReadBytes, void* buffer, size_t bufferSize);
};

}}