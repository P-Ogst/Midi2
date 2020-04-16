#include <midici\midi2_MidiCiMessages.h>
#include <stdio.h>

namespace midi2 { namespace midici {

namespace
{
void WriteToBuffer(void* buffer, size_t bufferSize, uint32_t value)
{
    if (bufferSize < sizeof(uint32_t))
    {
        return;
    }

    char valueBuffer[sizeof(uint32_t) + 1];
    sprintf_s(valueBuffer, "%d", value);
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    for (int i = 0; i < sizeof(uint32_t); ++i)
    {
        *bufferPtr = valueBuffer[i];
        bufferPtr++;
    }
}
}

UniversalSysExMessageBase::UniversalSysExMessageBase(MessageType messageId, DeviceId deviceId, uint32_t sourceMuid, uint32_t destMuid)
    : m_MessageType(messageId)
    , m_DeviceId(deviceId)
    , m_SourceMuid(sourceMuid)
    , m_DestMuid(destMuid)
{
}

MessageType UniversalSysExMessageBase::GetMessageType()
{
    return m_MessageType;
}

size_t UniversalSysExMessageBase::GetMessageSize()
{
    return 15 + GetDataSize();
}

int UniversalSysExMessageBase::Write(void* buffer, size_t bufferSize)
{
    if (bufferSize < GetMessageSize())
    {
        return -1;
    }

    char* bufferPtr = reinterpret_cast<char*>(buffer);
    // System Exclusive Start
    *bufferPtr = static_cast<char>(0xF0);
    bufferPtr++;
    // Universal System Exclusive
    *bufferPtr = 0x7E;
    bufferPtr++;
    // Device ID
    *bufferPtr = static_cast<char>(m_DeviceId);
    bufferPtr++;
    // Sub-ID#1
    *bufferPtr = static_cast<char>(SubId1::MidiCi);
    bufferPtr++;
    // Sub-ID#2 (MessageType)
    *bufferPtr = static_cast<char>(m_MessageType);
    bufferPtr++;
    // MIDI-CI MessageVersion (indicates MIDI-CI version 1.1)
    *bufferPtr = 0x01;
    bufferPtr++;
    // TODO: Add MUID Write process
    WriteToBuffer(bufferPtr, 4, m_SourceMuid);
    bufferPtr += 4;
    WriteToBuffer(bufferPtr, 4, m_DestMuid);
    bufferPtr += 4;

    OnDataWritten(bufferPtr, bufferSize - 15);
    bufferPtr += GetDataSize();

    // End Universal System Exclusive
    *bufferPtr = static_cast<char>(0xF7);
    bufferPtr++;
    return GetMessageSize();
}

void UniversalSysExMessageBase::Dump()
{
    char buf[4096];
    auto writeBytes = Write(buf, sizeof(buf));

    printf("MidiMessage: ");
    for (int i = 0; i < writeBytes; ++i)
    {
        printf("%x ", buf[i]);
    }
    printf("\n");
}

DiscoveryMessage::DiscoveryMessage(uint32_t sourceMuid, uint32_t destMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(MessageType::Discovery, DeviceId::MidiPort, sourceMuid, destMuid)
    , m_DeviceManufacturer(deviceManufacturer)
    , m_DeviceFamily(deviceFamily)
    , m_FamilyModelNumber(familyModelNumber)
    , m_SoftwareRevisionLevel(softwareRevisionLevel)
    , m_CategorySupported(categorySupported)
    , m_ReceivableMaximumSysExMessageSize(receivableMaximumSysExMessageSize)
{
}

ReplyToDiscoveryMessage::ReplyToDiscoveryMessage(uint32_t sourceMuid, uint32_t destMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(MessageType::ReplyToDiscovery, DeviceId::MidiPort, sourceMuid, destMuid)
    , m_DeviceManufacturer(deviceManufacturer)
    , m_DeviceFamily(deviceFamily)
    , m_FamilyModelNumber(familyModelNumber)
    , m_SoftwareRevisionLevel(softwareRevisionLevel)
    , m_CategorySupported(categorySupported)
    , m_ReceivableMaximumSysExMessageSize(receivableMaximumSysExMessageSize)
{
}

}
}