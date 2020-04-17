#include <midici\midi2_MidiCiMessages.h>
#include <stdio.h>

namespace midi2 { namespace midici {

namespace
{
template <class T>
void WriteToBuffer(void* buffer, size_t bufferSize, T value)
{
    if (bufferSize < sizeof(T))
    {
        return;
    }

    char* valuePtr = reinterpret_cast<char*>(&value);
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    for (int i = 0; i < sizeof(T); ++i)
    {
        *bufferPtr = *valuePtr;
        valuePtr++;
        bufferPtr++;
    }
}

template <class T>
void WriteToBufferAsUint24(void* buffer, size_t bufferSize, T value)
{
    if (bufferSize < 3)
    {
        return;
    }

    char* valuePtr = reinterpret_cast<char*>(&value);
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    for (int i = 0; i < 3; ++i)
    {
        *bufferPtr = *valuePtr;
        valuePtr++;
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
    return static_cast<size_t>(15) + GetDataSize();
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

    *bufferPtr = static_cast<char>(m_DeviceId);
    bufferPtr++;
    *bufferPtr = static_cast<char>(SubId1::MidiCi);
    bufferPtr++;
    *bufferPtr = static_cast<char>(m_MessageType);
    bufferPtr++;

    // MIDI-CI MessageVersion (indicates MIDI-CI version 1.1)
    *bufferPtr = 0x01;
    bufferPtr++;

    WriteToBuffer(bufferPtr, 4, m_SourceMuid);
    bufferPtr += 4;
    WriteToBuffer(bufferPtr, 4, m_DestMuid);
    bufferPtr += 4;

    // Message Data
    OnDataWritten(bufferPtr, bufferSize - 15);
    bufferPtr += GetDataSize();

    // End Universal System Exclusive
    *bufferPtr = static_cast<char>(0xF7);
    bufferPtr++;
    return static_cast<int>(GetMessageSize());
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

DiscoveryMessage::DiscoveryMessage(uint32_t sourceMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(MessageType::Discovery, DeviceId::MidiPort, sourceMuid, (uint32_t)Muid::BloadcastMuid)
    , m_DeviceManufacturer(deviceManufacturer)
    , m_DeviceFamily(deviceFamily)
    , m_FamilyModelNumber(familyModelNumber)
    , m_SoftwareRevisionLevel(softwareRevisionLevel)
    , m_CategorySupported(categorySupported)
    , m_ReceivableMaximumSysExMessageSize(receivableMaximumSysExMessageSize)
{
}

int DiscoveryMessage::GetDataSize()
{
    return 16;
}

void DiscoveryMessage::OnDataWritten(void* buffer, size_t bufferSize)
{
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    WriteToBufferAsUint24(bufferPtr, 3, m_DeviceManufacturer);
    bufferPtr += 3;
    WriteToBuffer(bufferPtr, 2, m_DeviceFamily);
    bufferPtr += 2;
    WriteToBuffer(bufferPtr, 2, m_FamilyModelNumber);
    bufferPtr += 2;
    WriteToBuffer(bufferPtr, 4, m_SoftwareRevisionLevel);
    bufferPtr += 4;
    WriteToBuffer(bufferPtr, 1, m_CategorySupported);
    bufferPtr += 1;
    WriteToBuffer(bufferPtr, 4, m_ReceivableMaximumSysExMessageSize);
    bufferPtr += 4;
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

int ReplyToDiscoveryMessage::GetDataSize()
{
    return 16;
}

void ReplyToDiscoveryMessage::OnDataWritten(void* buffer, size_t bufferSize)
{
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    WriteToBufferAsUint24(bufferPtr, 3, m_DeviceManufacturer);
    bufferPtr += 3;
    WriteToBuffer(bufferPtr, 2, m_DeviceFamily);
    bufferPtr += 2;
    WriteToBuffer(bufferPtr, 2, m_FamilyModelNumber);
    bufferPtr += 2;
    WriteToBuffer(bufferPtr, 4, m_SoftwareRevisionLevel);
    bufferPtr += 4;
    WriteToBuffer(bufferPtr, 1, m_CategorySupported);
    bufferPtr += 1;
    WriteToBuffer(bufferPtr, 4, m_ReceivableMaximumSysExMessageSize);
    bufferPtr += 4;
}

}
}