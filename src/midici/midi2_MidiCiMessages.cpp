#include <midici\midi2_MidiCiMessages.h>
#include <stdio.h>

namespace midi2 { namespace midici {

namespace
{

template <class T>
T ReadFromBuffer(void* buffer, size_t bufferSize, size_t valueSize)
{
    if (bufferSize < valueSize)
    {
        return 0;
    }

    T result = 0;
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    char* resultPtr = reinterpret_cast<char*>(&result);
    for (int i = 0; i < valueSize; ++i)
    {
        *resultPtr = *bufferPtr;
        bufferPtr++;
        resultPtr++;
    }
    return result;
}

template <class T>
T ReadFromBuffer(void* buffer, size_t bufferSize)
{
    if (bufferSize < sizeof(T))
    {
        return 0;
    }

    T result = 0;
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    char* resultPtr = reinterpret_cast<char*>(&result);
    for (int i = 0; i < sizeof(T); ++i)
    {
        *resultPtr = *bufferPtr;
        bufferPtr++;
        resultPtr++;
    }
    return result;
}

template <class T>
void WriteToBuffer(void* buffer, size_t bufferSize, T value, size_t valueSize)
{
    if (bufferSize < valueSize)
    {
        return;
    }

    char* valuePtr = reinterpret_cast<char*>(&value);
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    for (int i = 0; i < valueSize; ++i)
    {
        *bufferPtr = *valuePtr;
        valuePtr++;
        bufferPtr++;
    }
}

template <class T>
void WriteToBuffer(void* buffer, size_t bufferSize, T value)
{
    WriteToBuffer(buffer, bufferSize, value, sizeof(T));
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

size_t UniversalSysExMessageBase::GetMessageSizeMin()
{
    return 15u;
}

int UniversalSysExMessageBase::Write(void* buffer, size_t bufferSize)
{
    if (bufferSize < GetMessageSize() || GetMessageType() == MessageType::Invalid)
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

int UniversalSysExMessageBase::Read(void* buffer, size_t bufferSize)
{
    if (bufferSize < GetMessageSize())
    {
        return -1;
    }

    auto bufferPtr = reinterpret_cast<char*>(buffer);
    // SystemExclusive Start
    if (*bufferPtr != static_cast<char>(0xF0))
    {
        return -1;
    }
    bufferPtr++;
    // Universal System Exclusive
    if (*bufferPtr != static_cast<char>(0x7E))
    {
        return -1;
    }
    bufferPtr++;

    m_DeviceId = static_cast<DeviceId>(*bufferPtr);
    bufferPtr++;

    // Sub-ID(MIDI-CI)
    if (*bufferPtr != static_cast<char>(SubId1::MidiCi))
    {
        return -1;
    }
    bufferPtr++;

    auto messageType = static_cast<MessageType>(*bufferPtr);
    if (m_MessageType != messageType)
    {
        return -1;
    }
    bufferPtr++;

    auto version = static_cast<char>(*bufferPtr);
    if (version != 0x01)
    {
        return -1;
    }
    bufferPtr++;

    m_SourceMuid = ReadFromBuffer<uint32_t>(bufferPtr, 4);
    bufferPtr += 4;
    
    m_DestMuid = ReadFromBuffer<uint32_t>(bufferPtr, 4);
    bufferPtr += 4;

    if (!OnDataRead(bufferPtr, GetDataSize()))
    {
        return -1;
    }
    bufferPtr += GetDataSize();

    if (*bufferPtr != static_cast<char>(0xF7))
    {
        return -1;
    }

    return GetMessageSize();
}

void UniversalSysExMessageBase::Dump()
{
    if (GetMessageType() == MessageType::Invalid)
    {
        printf("Invalid Message\n");
        return;
    }
    char buf[4096];
    auto writeBytes = Write(buf, sizeof(buf));

    printf("MidiMessage: ");
    for (int i = 0; i < writeBytes; ++i)
    {
        printf("%02x ", static_cast<unsigned char>(buf[i]));
    }
    printf("\n");
}

DiscoveryMessage::DiscoveryMessage()
    : DiscoveryMessage(static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), static_cast<uint16_t>(-1), static_cast<uint16_t>(-1), static_cast<uint32_t>(-1), midi2::midici::CiCategorySupportedBitFlag::None, static_cast<uint32_t>(-1))
{
}

DiscoveryMessage::DiscoveryMessage(uint32_t sourceMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(MessageType::Discovery, DeviceId::MidiPort, sourceMuid, static_cast<uint32_t>(Muid::BloadcastMuid))
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
    WriteToBuffer(bufferPtr, 3, m_DeviceManufacturer, 3);
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

bool DiscoveryMessage::OnDataRead(void* buffer, size_t bufferSize)
{
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    m_DeviceManufacturer = ReadFromBuffer<uint32_t>(bufferPtr, 3, 3);
    bufferPtr += 3;
    m_DeviceFamily = ReadFromBuffer<uint16_t>(bufferPtr, 2);
    bufferPtr += 2;
    m_FamilyModelNumber = ReadFromBuffer<uint16_t>(bufferPtr, 2);
    bufferPtr += 2;
    m_SoftwareRevisionLevel = ReadFromBuffer<uint32_t>(bufferPtr, 4);
    bufferPtr += 4;
    m_CategorySupported = static_cast<CiCategorySupportedBitFlag>(*bufferPtr);
    bufferPtr++;
    m_ReceivableMaximumSysExMessageSize = ReadFromBuffer<uint32_t>(bufferPtr, 4);

    return true;
}

ReplyToDiscoveryMessage::ReplyToDiscoveryMessage()
    : ReplyToDiscoveryMessage(static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), static_cast<uint16_t>(-1), static_cast<uint16_t>(-1), static_cast<uint32_t>(-1), midi2::midici::CiCategorySupportedBitFlag::None, static_cast<uint32_t>(-1))
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

int ReplyToDiscoveryMessage::GetDataSize()
{
    return 16;
}

void ReplyToDiscoveryMessage::OnDataWritten(void* buffer, size_t bufferSize)
{
    char* bufferPtr = reinterpret_cast<char*>(buffer);
    WriteToBuffer(bufferPtr, 3, m_DeviceManufacturer, 3);
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

bool ReplyToDiscoveryMessage::OnDataRead(void* buffer, size_t bufferSize)
{
    return false;
}

InvalidMessage::InvalidMessage()
    : UniversalSysExMessageBase(MessageType::Invalid, static_cast<DeviceId>(-1), static_cast<uint32_t>(-1), static_cast<uint32_t>(-1))
{
}

int InvalidMessage::GetDataSize()
{
    return 0;
}

void InvalidMessage::OnDataWritten(void* buffer, size_t bufferSize)
{
}

bool InvalidMessage::OnDataRead(void* buffer, size_t bufferSize)
{
    return false;
}

}
}