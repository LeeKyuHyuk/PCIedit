#include <Windows.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "capability.h"
#include "capabilities/configuration_space_header.h"
#include "capabilities/pci_power_management_interface.h"
#include "capabilities/pci_express.h"
#include "pci.h"

using namespace std;

extern CommonConfigurationSpace common_configuration_space;

vector<BYTE> capability_order;

void CapabilityHighlight(const char* name, unsigned short offset) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 112);
	cout << "[+] [" << setfill('0') << setw(3) << right << uppercase << hex << offset << "] " << name << endl;
	SetConsoleTextAttribute(console, 7);
}

void ValueHighlight(BOOL expand, const char* name, unsigned long value, unsigned short size) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (expand)
		cout << "   [+] " << name << " : ";
	else
		cout << "    -  " << name << " : ";
	SetConsoleTextAttribute(console, 10);
	cout << "0x" << setfill('0') << setw(size) << right << uppercase << hex << value << endl;
	SetConsoleTextAttribute(console, 7);
}

unsigned short GetRegisterValue(unsigned long capability, unsigned long select, unsigned short shift) {
	return (capability & select) >> shift;
}

void PrintRegisterValue(BOOL newline, const char* name, const char* attribute, unsigned short value) {
	cout << "       -  " << name << " (" << attribute << ") : " << value;
	if (newline)
		cout << endl;
}

void PrintRegisterDescription(const char* description) {
	cout << " (" << description << ")" << endl;
}

void GetStandardCapabilities(int bus, int device, int function) {
	GetCommonConfigurationSpace(bus, device, function);
	if ((common_configuration_space.header_type & 0x7F) == 0x00)
		GetType0ConfigurationSpaceHeader(bus, device, function);
	if ((common_configuration_space.header_type & 0x7F) == 0x01)
		GetType1ConfigurationSpaceHeader(bus, device, function);
	BYTE next_item_pointer = common_configuration_space.capabilities_pointer;
	while (TRUE) {
		BYTE capability_id = GetCapabilityID(bus, device, function, next_item_pointer);
		capability_order.push_back(capability_id);
		/* printf("[DEBUG] Capability ID : 0x%02X\n", capability_id); */
		if (capability_id == CAPABILITY_PCI_POWER_MANAGEMENT_INTERFACE)
			GetPciPowerManagementInterfaceCapability(bus, device, function, next_item_pointer);
		if (capability_id == CAPABILITY_AGP)
			;
		if (capability_id == CAPABILITY_VPD)
			;
		if (capability_id == CAPABILITY_SLOT_IDENTIFICATION)
			;
		if (capability_id == CAPABILITY_MSI)
			;
		if (capability_id == CAPABILITY_COMPACTPCI_HOT_SWAP)
			;
		if (capability_id == CAPABILITY_PCI_X)
			;
		if (capability_id == CAPABILITY_HYPER_TRANSPORT)
			;
		if (capability_id == CAPABILITY_VENDOR_SPECIFIC)
			;
		if (capability_id == CAPABILITY_DEBUG_PORT)
			;
		if (capability_id == CAPABILITY_COMPACT_PCI_CENTRAL_RESOURCE_CONTROL)
			;
		if (capability_id == CAPABILITY_PCI_HOT_PLUG)
			;
		if (capability_id == CAPABILITY_PCI_BRIDGE_SUBSYSTEM_ID)
			;
		if (capability_id == CAPABILITY_AGP_8X)
			;
		if (capability_id == CAPABILITY_SECURE_DEVICE)
			;
		if (capability_id == CAPABILITY_PCI_EXPRESS)
			GetPciExpressCapability(bus, device, function, next_item_pointer);
		if (capability_id == CAPABILITY_MSI_X)
			;
		if (capability_id == CAPABILITY_SERIAL_ATA_DATA_INDEX_CONFIGURATION)
			;
		if (capability_id == CAPABILITY_ADVANCED_FEATURES)
			;
		if (capability_id == CAPABILITY_ENHANCED_ALLOCATION)
			;
		if (capability_id == CAPABILITY_FLATTENING_PORTAL_BRIDGE)
			;
		next_item_pointer = GetNextItemPointer(bus, device, function, next_item_pointer);
		if (next_item_pointer == 0)
			break;
		/* printf("[DEBUG] Next Item Pointer : 0x%02X\n", next_item_pointer); */
	}
}

BYTE GetCapabilityID(int bus, int device, int function, BYTE offset) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	ReadPciDword(bus, device, function, offset + 0x00, &data);
	return LOBYTE(data[1]);
}

BYTE GetNextItemPointer(int bus, int device, int function, BYTE offset) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	ReadPciDword(bus, device, function, offset + 0x00, &data);
	return HIBYTE(LOWORD(data[1]));
}

string GetVendorName(WORD vendor_id) {
	switch (vendor_id) {
	case 0x1002:
		return "ATI";
	case 0x10DE:
		return "NVIDIA";
	case 0x1022:
		return "AMD";
	case 0x15AD:
		return "VMWare";
	case 0x8086:
		return "Intel";
	case 0x1C5C:
		return "SK hynix";
	case 0x144D:
		return "Samsung";
	case 0x5143:
		return "Qualcomm";
	case 0x13B5:
		return "ARM";
	case 0x1AEE:
		return "Imagination Technologies";
	default:
		return "Unknown Vendor";
	}
}

string GetDeviceType(DWORD class_code) {
	switch (class_code >> 16) {
	case 0x00:
		return "Device was built before Class Code definitions were finalized";
	case 0x01:
		return "Mass Storage Controller";
	case 0x02:
		return "Network Controller";
	case 0x03:
		return "Display Controller";
	case 0x04:
		return "Multimedia Device";
	case 0x05:
		return "Memory Controller";
	case 0x06:
		return "Bridge Device";
	case 0x07:
		return "Simple Communication Controllers";
	case 0x08:
		return "Base System Peripherals";
	case 0x09:
		return "Input Devices";
	case 0x0A:
		return "Docking Stations";
	case 0x0B:
		return "Processors";
	case 0x0C:
		return "Serial Bus Controllers";
	case 0x0D:
		return "Wireless Controller";
	case 0x0E:
		return "Intelligent I/O Controllers";
	case 0x0F:
		return "Satellite Communication Controllers";
	case 0x10:
		return "Encryption/Decryption Controllers";
	case 0x11:
		return "Data Acquisition/Signal Processing Controller";
	case 0xFF:
		return "Device does not fit in any defined classes";
	default:
		return "Unknown Device Type";
	}
}

void PrintCapability(void) {
	if ((common_configuration_space.header_type & 0x7F) == 0x00)
		PrintType0ConfigurationSpaceHeader();
	if ((common_configuration_space.header_type & 0x7F) == 0x01)
		PrintType1ConfigurationSpaceHeader();
	for (unsigned int index = 0; index < capability_order.size(); index++) {
		BYTE capability_id = capability_order[index];
		if (capability_id == CAPABILITY_PCI_POWER_MANAGEMENT_INTERFACE)
			PrintPciPowerManagementInterfaceCapability();
		if (capability_id == CAPABILITY_AGP)
			;
		if (capability_id == CAPABILITY_VPD)
			;
		if (capability_id == CAPABILITY_SLOT_IDENTIFICATION)
			;
		if (capability_id == CAPABILITY_MSI)
			;
		if (capability_id == CAPABILITY_COMPACTPCI_HOT_SWAP)
			;
		if (capability_id == CAPABILITY_PCI_X)
			;
		if (capability_id == CAPABILITY_HYPER_TRANSPORT)
			;
		if (capability_id == CAPABILITY_VENDOR_SPECIFIC)
			;
		if (capability_id == CAPABILITY_DEBUG_PORT)
			;
		if (capability_id == CAPABILITY_COMPACT_PCI_CENTRAL_RESOURCE_CONTROL)
			;
		if (capability_id == CAPABILITY_PCI_HOT_PLUG)
			;
		if (capability_id == CAPABILITY_PCI_BRIDGE_SUBSYSTEM_ID)
			;
		if (capability_id == CAPABILITY_AGP_8X)
			;
		if (capability_id == CAPABILITY_SECURE_DEVICE)
			;
		if (capability_id == CAPABILITY_PCI_EXPRESS)
			PrintPciExpressCapability();
		if (capability_id == CAPABILITY_MSI_X)
			;
		if (capability_id == CAPABILITY_SERIAL_ATA_DATA_INDEX_CONFIGURATION)
			;
		if (capability_id == CAPABILITY_ADVANCED_FEATURES)
			;
		if (capability_id == CAPABILITY_ENHANCED_ALLOCATION)
			;
		if (capability_id == CAPABILITY_FLATTENING_PORTAL_BRIDGE)
			;
	}
}