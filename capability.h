#pragma once

#define CAPABILITY_PCI_POWER_MANAGEMENT_INTERFACE 0x01
#define CAPABILITY_AGP 0x02
#define CAPABILITY_VPD 0x03
#define CAPABILITY_SLOT_IDENTIFICATION 0x04
#define CAPABILITY_MSI 0x05
#define CAPABILITY_COMPACTPCI_HOT_SWAP 0x06
#define CAPABILITY_PCI_X 0x07
#define CAPABILITY_HYPER_TRANSPORT 0x08
#define CAPABILITY_VENDOR_SPECIFIC 0x09
#define CAPABILITY_DEBUG_PORT 0x0A
#define CAPABILITY_COMPACT_PCI_CENTRAL_RESOURCE_CONTROL 0x0B
#define CAPABILITY_PCI_HOT_PLUG 0x0C
#define CAPABILITY_PCI_BRIDGE_SUBSYSTEM_ID 0x0D
#define CAPABILITY_AGP_8X 0x0E
#define CAPABILITY_SECURE_DEVICE 0x0F
#define CAPABILITY_PCI_EXPRESS 0x10
#define CAPABILITY_MSI_X 0x11
#define CAPABILITY_SERIAL_ATA_DATA_INDEX_CONFIGURATION 0x12
#define CAPABILITY_ADVANCED_FEATURES 0x13
#define CAPABILITY_ENHANCED_ALLOCATION 0x14
#define CAPABILITY_FLATTENING_PORTAL_BRIDGE 0x15

void CapabilityHighlight(const char* name, unsigned short offset);
void ValueHighlight(BOOL expand, const char* name, unsigned long value, unsigned short size);
unsigned long GetRegisterValue(unsigned long capability, unsigned long select, unsigned short shift);
void PrintRegisterValue(BOOL newline, const char* name, const char* attribute, unsigned long value);
void PrintRegisterDescription(const char* description);
void GetStandardCapabilities(int bus, int device, int function);
BYTE GetCapabilityID(int bus, int device, int function, BYTE offset);
BYTE GetNextItemPointer(int bus, int device, int function, BYTE offset);
std::string GetVendorName(WORD vendor_id);
std::string GetDeviceType(DWORD class_code);
void PrintCapability(void);