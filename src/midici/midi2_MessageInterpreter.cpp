#include <midici\midi2_MessageInterpreter.h>

namespace midi2 { namespace midici {

MessageType MessageInterpreter::ReadMessageType(void* buffer, size_t bufferSize)
{
    if (bufferSize < UniversalSysExMessageBase::GetMessageSizeMin())
    {
        return MessageType::Invalid;
    }

    auto bufferPtr = reinterpret_cast<char*>(buffer);
    // SystemExclusive Start
    if (*bufferPtr != static_cast<char>(0xF0))
    {
        return MessageType::Invalid;
    }
    // Universal System Exclusive
    if (*(bufferPtr + 1) != static_cast<char>(0x7E))
    {
        return MessageType::Invalid;
    }

    // Sub-ID(MIDI-CI)
    if (*(bufferPtr + 3) != static_cast<char>(SubId1::MidiCi))
    {
        return MessageType::Invalid;
    }
    auto messageType = static_cast<MessageType>(*(bufferPtr + 4));
    for (size_t i = 5; i < bufferSize; ++i)
    {
        if (*(bufferPtr + i) == static_cast<char>(0xF7))
        {
            return messageType;
        }
    }
    return MessageType::Invalid;
}

}
}