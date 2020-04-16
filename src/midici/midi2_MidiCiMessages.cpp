#include <midici\midi2_MidiCiMessages.h>

namespace midi2 { namespace midici {
    
UniversalSysExMessageBase::UniversalSysExMessageBase(SubId2 messageId, DeviceId deviceId, uint32_t sourceMuid, uint32_t destMuid)
    : m_MessageId(messageId)
    , m_DeviceId(deviceId)
    , m_SourceMuid(sourceMuid)
    , m_DestMuid(destMuid)
{
}


DiscoveryMessage::DiscoveryMessage(uint32_t sourceMuid, uint32_t destMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(SubId2::Discovery, DeviceId::MidiPort, sourceMuid, destMuid)
    , m_DeviceManufacturer(deviceManufacturer)
    , m_DeviceFamily(deviceFamily)
    , m_FamilyModelNumber(familyModelNumber)
    , m_SoftwareRevisionLevel(softwareRevisionLevel)
    , m_CategorySupported(categorySupported)
    , m_ReceivableMaximumSysExMessageSize(receivableMaximumSysExMessageSize)
{
}

ReplyToDiscoveryMessage::ReplyToDiscoveryMessage(uint32_t sourceMuid, uint32_t destMuid, uint32_t deviceManufacturer, uint16_t deviceFamily, uint16_t familyModelNumber, uint32_t softwareRevisionLevel, CiCategorySupportedBitFlag categorySupported, uint32_t receivableMaximumSysExMessageSize)
    : UniversalSysExMessageBase(SubId2::ReplyToDiscovery, DeviceId::MidiPort, sourceMuid, destMuid)
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