#pragma once
#include <stdint.h>

namespace midi2 { namespace midici {

enum class DeviceId : char
{
    MidiPort = 0x7F,
    Channel1 = 0x00,
    Channel2 = 0x01,
    Channel3 = 0x02,
    Channel4 = 0x03,
    Channel5 = 0x04,
    Channel6 = 0x05,
    Channel7 = 0x06,
    Channel8 = 0x07,
    Channel9 = 0x08,
    Channel10 = 0x09,
    Channel11 = 0x0A,
    Channel12 = 0x0B,
    Channel13 = 0x0C,
    Channel14 = 0x0D,
    Channel15 = 0x0E,
    Channel16 = 0x0F,
};

enum class SubId1 : char
{
    MidiCi = 0x0D,
};

enum class SubId2 : char
{
    Discovery = 0x70,
    ReplyToDiscovery = 0x71,
};

class UniversalSysExMessageBase
{
public:
    UniversalSysExMessageBase(SubId2 messageId, DeviceId deviceId, uint32_t sourceMuid, uint32_t destMuid);
    virtual ~UniversalSysExMessageBase() {}

    int GetMessageSize();
    int Write(void* buffer, size_t bufferSize);
    void Dump();

protected:
    virtual int GetDataSize() { return 0; }
    virtual void OnDataWritten(void* buffer, size_t bufferSize) {}

private:
    SubId2 m_MessageId;
    DeviceId m_DeviceId;
    uint32_t m_SourceMuid;
    uint32_t m_DestMuid;
};

enum class CiCategorySupportedBitFlag : char
{
    ProtocolNegotiationSupported = 1 << 1,
    ProfileConfigurationSupported = 1 << 2,
    PropertyExchangeSupported = 1 << 3,
};

class DiscoveryMessage : UniversalSysExMessageBase
{
public:
    DiscoveryMessage(uint32_t sourceMuid,
                     uint32_t destMuid,
                     uint32_t deviceManufacturer,
                     uint16_t deviceFamily,
                     uint16_t familyModelNumber,
                     uint32_t softwareRevisionLevel,
                     CiCategorySupportedBitFlag categorySupported,
                     uint32_t receivableMaximumSysExMessageSize);

protected:
    virtual int GetDataSize() override;
    virtual void OnDataWritten(void* buffer, size_t bufferSize) override;

private:
    uint32_t m_DeviceManufacturer; // 3byte
    uint16_t m_DeviceFamily;
    uint16_t m_FamilyModelNumber;
    uint32_t m_SoftwareRevisionLevel;
    CiCategorySupportedBitFlag m_CategorySupported;
    uint32_t m_ReceivableMaximumSysExMessageSize;
};

class ReplyToDiscoveryMessage : UniversalSysExMessageBase
{
public:
    ReplyToDiscoveryMessage(uint32_t sourceMuid,
                            uint32_t destMuid,
                            uint32_t deviceManufacturer,
                            uint16_t deviceFamily,
                            uint16_t familyModelNumber,
                            uint32_t softwareRevisionLevel,
                            CiCategorySupportedBitFlag categorySupported,
                            uint32_t receivableMaximumSysExMessageSize);

protected:
    virtual int GetDataSize() override;
    virtual void OnDataWritten(void* buffer, size_t bufferSize) override;

private:
    uint32_t m_DeviceManufacturer; // 3byte
    uint16_t m_DeviceFamily;
    uint16_t m_FamilyModelNumber;
    uint32_t m_SoftwareRevisionLevel;
    CiCategorySupportedBitFlag m_CategorySupported;
    uint32_t m_ReceivableMaximumSysExMessageSize;
};

}}