#include <midici\midi2_MessageInterpreter.h>

namespace midi2 { namespace midici {

UniversalSysExMessageBase MessageInterpreter::ReadMessage(int* pOutReadBytes, void*buffer, size_t bufferSize)
{
    return Parse(pOutReadBytes, buffer, bufferSize);
}

UniversalSysExMessageBase MessageInterpreter::Parse(int* pOutReadBytes, void* buffer, size_t bufferSize)
{
    if (bufferSize < UniversalSysExMessageBase::GetMessageSizeMin())
    {
        return InvalidMessage();
    }

    auto bufferPtr = reinterpret_cast<char*>(buffer);
    // SystemExclusive Start
    if (*bufferPtr != static_cast<char>(0xF0))
    {
        return InvalidMessage();
    }
    // Universal System Exclusive
    if (*(bufferPtr + 1) != static_cast<char>(0x7E))
    {
        return InvalidMessage();
    }

    // Sub-ID(MIDI-CI)
    if (*(bufferPtr + 3) != static_cast<char>(SubId1::MidiCi))
    {
        return InvalidMessage();
    }
    auto messageType = static_cast<MessageType>(*(bufferPtr + 4));
    UniversalSysExMessageBase result = InvalidMessage();
    switch (messageType)
    {
    case MessageType::Discovery:
        result = DiscoveryMessage();
        break;
    case MessageType::ReplyToDiscovery:
        result = ReplyToDiscoveryMessage();
        break;
    default:
        return InvalidMessage();
    }
    *pOutReadBytes = result.Read(buffer, bufferSize);
    if (*pOutReadBytes < 0)
    {
        return InvalidMessage();
    }
    return result;
}

}
}