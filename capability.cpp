#include <Windows.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "capability.h"
#include "pci.h"

using namespace std;

typedef struct CommonConfigurationSpace {
	BOOL exists;
	WORD vendor_id;
	WORD device_id;
	WORD command;
	WORD status;
	BYTE revision_id;
	DWORD class_code;
	BYTE cache_line_size;
	BYTE latency_timer;
	BYTE header_type;
	BYTE bist;
	BYTE capabilities_pointer;
	BYTE interrupt_line;
	BYTE interrupt_pin;
} CommonConfigurationSpace;

typedef struct Type0ConfigurationSpaceHeader {
	BOOL exists;
	DWORD base_address_register[6];
	DWORD cardbus_cis_pointer;
	WORD subsystem_vendor_id;
	WORD subsystem_id;
	DWORD expansion_rom_base_address;
	DWORD reserved1;
	DWORD reserved2;
	BYTE min_grant;
	BYTE max_latency;
} Type0ConfigurationSpaceHeader;

typedef struct Type1ConfigurationSpaceHeader {
	BOOL exists;
	DWORD base_address_register[2];
	BYTE primary_bus_number;
	BYTE secondary_bus_number;
	BYTE subordinate_bus_number;
	BYTE secondary_latency_timer;
	BYTE io_base;
	BYTE io_limit;
	WORD secondary_status;
	WORD memory_base;
	WORD memory_limit;
	WORD prefetchable_memory_base;
	WORD prefetchable_memory_limit;
	DWORD prefetchable_memory_base_upper;
	DWORD prefetchable_memory_limit_upper;
	WORD io_base_upper;
	WORD io_limit_upper;
	DWORD expansion_rom_base_address;
	WORD bridge_control;
} Type1ConfigurationSpaceHeader;

typedef struct PciPowerManagementInterfaceCapability {
	BOOL exists;
	BYTE offset;
	BYTE capability_id;
	BYTE next_capability_pointer;
	WORD power_management_capabilities;
	WORD power_management_control_status;
	BYTE reserved;
	BYTE data;
} PciPowerManagementInterfaceCapability;

vector<BYTE> capability_order;

CommonConfigurationSpace common_configuration_space = { 0, };
Type0ConfigurationSpaceHeader type_0_configuration_space_header = { 0, };
Type1ConfigurationSpaceHeader type_1_configuration_space_header = { 0, };
PciPowerManagementInterfaceCapability pci_power_management_interface_capability = { 0, };

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

string ReverseString(const string str) {
	string reverse;
	for (unsigned int index = 0; index < str.length(); index++)
		reverse += str.substr(str.length() - index - 1, 1);
	return reverse;
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
			;
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

void GetCommonConfigurationSpace(int bus, int device, int function) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	ReadPciDword(bus, device, function, 0x00, &data);
	common_configuration_space.vendor_id = LOWORD(data[1]);
	common_configuration_space.device_id = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x04, &data);
	common_configuration_space.command = LOWORD(data[1]);
	common_configuration_space.status = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x08, &data);
	common_configuration_space.revision_id = LOBYTE(data[1]);
	common_configuration_space.class_code = data[1] >> 8;
	ReadPciDword(bus, device, function, 0x0C, &data);
	common_configuration_space.cache_line_size = LOBYTE(LOWORD(data[1]));
	common_configuration_space.latency_timer = HIBYTE(LOWORD(data[1]));
	common_configuration_space.header_type = LOBYTE(HIWORD(data[1]));
	common_configuration_space.bist = HIBYTE(HIWORD(data[1]));
	ReadPciDword(bus, device, function, 0x34, &data);
	common_configuration_space.capabilities_pointer = LOBYTE(data[1]);
	ReadPciDword(bus, device, function, 0x3C, &data);
	common_configuration_space.interrupt_line = LOBYTE(LOWORD(data[1]));
	common_configuration_space.interrupt_pin = HIBYTE(LOWORD(data[1]));
	common_configuration_space.exists = TRUE;
}

void GetType0ConfigurationSpaceHeader(int bus, int device, int function) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	ReadPciDword(bus, device, function, 0x10, &data);
	type_0_configuration_space_header.base_address_register[0] = data[1];
	ReadPciDword(bus, device, function, 0x14, &data);
	type_0_configuration_space_header.base_address_register[1] = data[1];
	ReadPciDword(bus, device, function, 0x18, &data);
	type_0_configuration_space_header.base_address_register[2] = data[1];
	ReadPciDword(bus, device, function, 0x1C, &data);
	type_0_configuration_space_header.base_address_register[3] = data[1];
	ReadPciDword(bus, device, function, 0x20, &data);
	type_0_configuration_space_header.base_address_register[4] = data[1];
	ReadPciDword(bus, device, function, 0x24, &data);
	type_0_configuration_space_header.base_address_register[5] = data[1];
	ReadPciDword(bus, device, function, 0x28, &data);
	type_0_configuration_space_header.cardbus_cis_pointer = data[1];
	ReadPciDword(bus, device, function, 0x2C, &data);
	type_0_configuration_space_header.subsystem_vendor_id = LOWORD(data[1]);
	type_0_configuration_space_header.subsystem_id = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x30, &data);
	type_0_configuration_space_header.expansion_rom_base_address = data[1];
	ReadPciDword(bus, device, function, 0x34, &data);
	type_0_configuration_space_header.reserved1 = data[1] >> 8;
	ReadPciDword(bus, device, function, 0x38, &data);
	type_0_configuration_space_header.reserved2 = data[1];
	ReadPciDword(bus, device, function, 0x3C, &data);
	type_0_configuration_space_header.min_grant = LOBYTE(HIWORD(data[1]));
	type_0_configuration_space_header.max_latency = HIBYTE(HIWORD(data[1]));
	type_0_configuration_space_header.exists = TRUE;
}

void GetType1ConfigurationSpaceHeader(int bus, int device, int function) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	ReadPciDword(bus, device, function, 0x10, &data);
	type_1_configuration_space_header.base_address_register[0] = data[1];
	ReadPciDword(bus, device, function, 0x14, &data);
	type_1_configuration_space_header.base_address_register[1] = data[1];
	ReadPciDword(bus, device, function, 0x18, &data);
	type_1_configuration_space_header.primary_bus_number = LOBYTE(LOWORD(data[1]));
	type_1_configuration_space_header.secondary_bus_number = HIBYTE(LOWORD(data[1]));
	type_1_configuration_space_header.subordinate_bus_number = LOBYTE(HIWORD(data[1]));
	type_1_configuration_space_header.secondary_latency_timer = HIBYTE(HIWORD(data[1]));
	ReadPciDword(bus, device, function, 0x1C, &data);
	type_1_configuration_space_header.io_base = LOBYTE(LOWORD(data[1]));
	type_1_configuration_space_header.io_limit = HIBYTE(LOWORD(data[1]));
	type_1_configuration_space_header.secondary_status = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x20, &data);
	type_1_configuration_space_header.memory_base = LOWORD(data[1]);
	type_1_configuration_space_header.memory_limit = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x24, &data);
	type_1_configuration_space_header.prefetchable_memory_base = LOWORD(data[1]);
	type_1_configuration_space_header.prefetchable_memory_limit = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x28, &data);
	type_1_configuration_space_header.prefetchable_memory_base_upper = data[1];
	ReadPciDword(bus, device, function, 0x2C, &data);
	type_1_configuration_space_header.prefetchable_memory_limit_upper = data[1];
	ReadPciDword(bus, device, function, 0x30, &data);
	type_1_configuration_space_header.io_base_upper = LOWORD(data[1]);
	type_1_configuration_space_header.io_limit_upper = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x38, &data);
	type_1_configuration_space_header.expansion_rom_base_address = data[1];
	ReadPciDword(bus, device, function, 0x3C, &data);
	type_1_configuration_space_header.bridge_control = HIWORD(data[1]);
	type_1_configuration_space_header.exists = TRUE;
}

void GetPciPowerManagementInterfaceCapability(int bus, int device, int function, BYTE offset) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	pci_power_management_interface_capability.offset = offset;
	ReadPciDword(bus, device, function, offset + 0x00, &data);
	pci_power_management_interface_capability.capability_id = LOBYTE(data[1]);
	pci_power_management_interface_capability.next_capability_pointer = HIBYTE(LOWORD(data[1]));
	pci_power_management_interface_capability.power_management_capabilities = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x04, &data);
	pci_power_management_interface_capability.power_management_control_status = LOWORD(data[1]);
	pci_power_management_interface_capability.reserved = LOBYTE(HIWORD(data[1]));
	pci_power_management_interface_capability.data = HIBYTE(data[1]);
	pci_power_management_interface_capability.exists = TRUE;
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
			;
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

void PrintType0ConfigurationSpaceHeader(void) {
	string bit;
	if (type_0_configuration_space_header.exists) {
		CapabilityHighlight("Type 0 Configuration Space Header", 0x00);
		ValueHighlight(FALSE, "Vendor ID", common_configuration_space.vendor_id, 4);
		ValueHighlight(FALSE, "Device ID", common_configuration_space.device_id, 4);
		ValueHighlight(TRUE, "Command", common_configuration_space.command, 4);
		bit = ReverseString(bitset<16>(common_configuration_space.command).to_string());
		cout << "       -  I/O Space Enable (RW) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Disables the device response)" << endl;
		if (bit.at(0) == '1')
			cout << " (Allows the device to respond to I/O Space accesses)" << endl;
		cout << "       -  Memory Space Enable (RW) : " << bit.at(1);
		if (bit.at(1) == '0')
			cout << " (Disables the device response)" << endl;
		if (bit.at(1) == '1')
			cout << " (Allows the device to respond to Memory Space accesses)" << endl;
		cout << "       -  Bus Master Enable (RW) : " << bit.at(2);
		if (bit.at(2) == '0')
			cout << " (PCI Express Function is not allowed to issue any Memory or I/O Requests)" << endl;
		if (bit.at(2) == '1')
			cout << " (PCI Express Function is allowed to issue Memory or I/O Requests)" << endl;
		cout << "       -  Special Cycle Enable (RO) : " << bit.at(3) << endl;
		cout << "       -  Memory Write and Invalidate (RO) : " << bit.at(4) << endl;
		cout << "       -  VGA Palette Snoop (RO) : " << bit.at(5) << endl;
		cout << "       -  Parity Error Response (RW) : " << bit.at(6) << endl;
		cout << "       -  IDSEL Stepping/Wait Cycle Control (RO) : " << bit.at(7) << endl;
		cout << "       -  SERR# Enable (RW) : " << bit.at(8) << endl;
		cout << "       -  Fast Back-to-Back Transactions Enable (RO) : " << bit.at(9) << endl;
		cout << "       -  Interrupt Disable (RW) :" << bit.at(10) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(11, 5)) << endl;
		ValueHighlight(TRUE, "Status", common_configuration_space.status, 4);
		bit = ReverseString(bitset<16>(common_configuration_space.status).to_string());
		cout << "       -  Immediate Readiness (RO) : " << bit.at(0) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(1, 2)) << endl;
		cout << "       -  Interrupt Status (RO) : " << bit.at(3) << endl;
		cout << "       -  Capabilities List (RO) : " << bit.at(4) << endl;
		cout << "       -  66 MHz Capable (RO) : " << bit.at(5) << endl;
		cout << "       -  Reserved (RSVD) : " << bit.at(6) << endl;
		cout << "       -  Fast Back-to-Back Transactions Capable (RO) : " << bit.at(7) << endl;
		cout << "       -  Master Data Parity Error (RW1C) : " << bit.at(8) << endl;
		cout << "       -  DEVSEL Timing (RO) : " << ReverseString(bit.substr(9, 2)) << endl;
		cout << "       -  Signaled Target Abort (RW1C) : " << bit.at(11) << endl;
		cout << "       -  Received Target Abort (RW1C)  : " << bit.at(12) << endl;
		cout << "       -  Received Master Abort (RW1C) : " << bit.at(13) << endl;
		cout << "       -  Signaled System Error (RW1C) : " << bit.at(14) << endl;
		cout << "       -  Detected Parity Error (RW1C) : " << bit.at(15) << endl;
		ValueHighlight(FALSE, "Revision ID", common_configuration_space.revision_id, 2);
		ValueHighlight(TRUE, "Class Code", common_configuration_space.class_code, 6);
		bit = ReverseString(bitset<24>(common_configuration_space.class_code).to_string());
		cout << "       -  Programming Interface (RO) : " << ReverseString(bit.substr(0, 8)) << endl;
		cout << "       -  Sub-Class Code (RO) : " << ReverseString(bit.substr(8, 8)) << endl;
		cout << "       -  Base Class Code (RO) : " << ReverseString(bit.substr(16, 8));
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x00)
			cout << " (Device was built before Class Code definitions were finalized)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x01)
			cout << " (Mass storage controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x02)
			cout << " (Network controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x03)
			cout << " (Display controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x04)
			cout << " (Multimedia device)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x05)
			cout << " (Memory controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x06)
			cout << " (Bridge device)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x07)
			cout << " (Simple communication controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x08)
			cout << " (Base system peripherals)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x09)
			cout << " (Input devices)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0A)
			cout << " (Docking stations)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0B)
			cout << " (Processors)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0C)
			cout << " (Serial bus controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0D)
			cout << " (Wireless controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0E)
			cout << " (Intelligent I/O controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0F)
			cout << " (Satellite communication controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x10)
			cout << " (Encryption/Decryption controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x11)
			cout << " (Data acquisition and signal processing controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0xFF)
			cout << " (Device does not fit in any defined classes)" << endl;
		ValueHighlight(FALSE, "Cache Line Size", common_configuration_space.cache_line_size, 2);
		ValueHighlight(FALSE, "Lantency Timer", common_configuration_space.latency_timer, 2);
		ValueHighlight(TRUE, "Header Type", common_configuration_space.header_type, 2);
		bit = ReverseString(bitset<8>(common_configuration_space.header_type).to_string());
		cout << "       -  Header Layout (RO) : " << ReverseString(bit.substr(0, 7));
		if (stoi(ReverseString(bit.substr(0, 7)), 0, 2) == 0x00)
			cout << " (Type 0 Configuration Space Header)" << endl;
		if (stoi(ReverseString(bit.substr(0, 7)), 0, 2) == 0x01)
			cout << " (Type 1 Configuration Space Header)" << endl;
		cout << "       -  Multi-Function Device (RO) : " << bit.at(7);
		if (bit.at(7) == '0')
			cout << " (Device is single function)" << endl;
		if (bit.at(7) == '1')
			cout << " (Device has multiple functions)" << endl;
		ValueHighlight(TRUE, "BIST", common_configuration_space.bist, 2);
		bit = ReverseString(bitset<8>(common_configuration_space.bist).to_string());
		cout << "       -  Completion Code (RO) : " << ReverseString(bit.substr(0, 4)) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(4, 2)) << endl;
		cout << "       -  Start BIST (RW) : " << bit.at(6) << endl;
		cout << "       -  BIST Capable (RO) : " << bit.at(7);
		if (bit.at(7) == '0')
			cout << " (Device is not BIST capable)" << endl;
		if (bit.at(7) == '1')
			cout << " (Device supports BIST)" << endl;
		ValueHighlight(TRUE, "Base Address Register 0", type_0_configuration_space_header.base_address_register[0], 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.base_address_register[0]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		ValueHighlight(FALSE, "Base Address Register 1", type_0_configuration_space_header.base_address_register[1], 8);
		ValueHighlight(TRUE, "Base Address Register 2", type_0_configuration_space_header.base_address_register[2], 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.base_address_register[2]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		ValueHighlight(TRUE, "Base Address Register 3", type_0_configuration_space_header.base_address_register[3], 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.base_address_register[3]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		ValueHighlight(TRUE, "Base Address Register 4", type_0_configuration_space_header.base_address_register[4], 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.base_address_register[4]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		ValueHighlight(TRUE, "Base Address Register 5", type_0_configuration_space_header.base_address_register[5], 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.base_address_register[5]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		ValueHighlight(FALSE, "Cardbus CIS Pointer", type_0_configuration_space_header.cardbus_cis_pointer, 8);
		ValueHighlight(FALSE, "Subsystem Vendor ID", type_0_configuration_space_header.subsystem_vendor_id, 4);
		ValueHighlight(FALSE, "Subsystem ID", type_0_configuration_space_header.subsystem_id, 4);
		ValueHighlight(TRUE, "Expansion ROM Base Address", type_0_configuration_space_header.expansion_rom_base_address, 8);
		bit = ReverseString(bitset<32>(type_0_configuration_space_header.expansion_rom_base_address).to_string());
		cout << "       -  Expansion ROM Enable (RW) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (The device\'s expansion ROM address space is disabled)" << endl;
		if (bit.at(0) == '1')
			cout << " (Address decoding is enabled using the parameters in the other part of the base register)" << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(1, 10)) << endl;
		cout << "       -  Expansion ROM Base Address (RW) : " << ReverseString(bit.substr(11, 21)) << endl;
		ValueHighlight(FALSE, "Capabilities Pointer", common_configuration_space.capabilities_pointer, 2);
		ValueHighlight(FALSE, "Reserved", type_0_configuration_space_header.reserved1, 6);
		ValueHighlight(FALSE, "Reserved", type_0_configuration_space_header.reserved2, 8);
		ValueHighlight(FALSE, "Interrupt Line", common_configuration_space.interrupt_line, 2);
		ValueHighlight(FALSE, "Interrupt Pin", common_configuration_space.interrupt_pin, 2);
		ValueHighlight(FALSE, "Min Grant", type_0_configuration_space_header.min_grant, 2);
		ValueHighlight(FALSE, "Max Latency", type_0_configuration_space_header.max_latency, 2);
	}
}

void PrintType1ConfigurationSpaceHeader(void) {
	string bit;
	if (type_1_configuration_space_header.exists) {
		CapabilityHighlight("Type 1 Configuration Space Header", 0x00);
		printf("    -  Vendor ID : 0x%04X\n", common_configuration_space.vendor_id);
		printf("    -  Device ID : 0x%04X\n", common_configuration_space.device_id);
		printf("   [+] Command : 0x%04X\n", common_configuration_space.command);
		bit = ReverseString(bitset<16>(common_configuration_space.command).to_string());
		cout << "       -  I/O Space Enable (RW) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Disables the device response)" << endl;
		if (bit.at(0) == '1')
			cout << " (Allows the device to respond to I/O Space accesses)" << endl;
		cout << "       -  Memory Space Enable (RW) : " << bit.at(1);
		if (bit.at(1) == '0')
			cout << " (Disables the device response)" << endl;
		if (bit.at(1) == '1')
			cout << " (Allows the device to respond to Memory Space accesses)" << endl;
		cout << "       -  Bus Master Enable (RW) : " << bit.at(2);
		if (bit.at(2) == '0')
			cout << " (PCI Express Function is not allowed to issue any Memory or I/O Requests)" << endl;
		if (bit.at(2) == '1')
			cout << " (PCI Express Function is allowed to issue Memory or I/O Requests)" << endl;
		cout << "       -  Special Cycle Enable (RO) : " << bit.at(3) << endl;
		cout << "       -  Memory Write and Invalidate (RO) : " << bit.at(4) << endl;
		cout << "       -  VGA Palette Snoop (RO) : " << bit.at(5) << endl;
		cout << "       -  Parity Error Response (RW) : " << bit.at(6) << endl;
		cout << "       -  IDSEL Stepping/Wait Cycle Control (RO) : " << bit.at(7) << endl;
		cout << "       -  SERR# Enable (RW) : " << bit.at(8) << endl;
		cout << "       -  Fast Back-to-Back Transactions Enable (RO) : " << bit.at(9) << endl;
		cout << "       -  Interrupt Disable (RW) :" << bit.at(10) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(11, 5)) << endl;
		printf("   [+] Status : 0x%04X\n", common_configuration_space.status);
		bit = ReverseString(bitset<16>(common_configuration_space.status).to_string());
		cout << "       -  Immediate Readiness (RO) : " << bit.at(0) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(1, 2)) << endl;
		cout << "       -  Interrupt Status (RO) : " << bit.at(3) << endl;
		cout << "       -  Capabilities List (RO) : " << bit.at(4) << endl;
		cout << "       -  66 MHz Capable (RO) : " << bit.at(5) << endl;
		cout << "       -  Reserved (RSVD) : " << bit.at(6) << endl;
		cout << "       -  Fast Back-to-Back Transactions Capable (RO) : " << bit.at(7) << endl;
		cout << "       -  Master Data Parity Error (RW1C) : " << bit.at(8) << endl;
		cout << "       -  DEVSEL Timing (RO) : " << ReverseString(bit.substr(9, 2)) << endl;
		cout << "       -  Signaled Target Abort (RW1C) : " << bit.at(11) << endl;
		cout << "       -  Received Target Abort (RW1C)  : " << bit.at(12) << endl;
		cout << "       -  Received Master Abort (RW1C) : " << bit.at(13) << endl;
		cout << "       -  Signaled System Error (RW1C) : " << bit.at(14) << endl;
		cout << "       -  Detected Parity Error (RW1C) : " << bit.at(15) << endl;
		printf("    -  Revision ID : 0x%02X\n", common_configuration_space.revision_id);
		printf("   [+] Class Code : 0x%06X\n", common_configuration_space.class_code);
		bit = ReverseString(bitset<24>(common_configuration_space.class_code).to_string());
		cout << "       -  Programming Interface (RO) : " << ReverseString(bit.substr(0, 8)) << endl;
		cout << "       -  Sub-Class Code (RO) : " << ReverseString(bit.substr(8, 8)) << endl;
		cout << "       -  Base Class Code (RO) : " << ReverseString(bit.substr(16, 8));
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x00)
			cout << " (Device was built before Class Code definitions were finalized)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x01)
			cout << " (Mass storage controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x02)
			cout << " (Network controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x03)
			cout << " (Display controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x04)
			cout << " (Multimedia device)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x05)
			cout << " (Memory controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x06)
			cout << " (Bridge device)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x07)
			cout << " (Simple communication controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x08)
			cout << " (Base system peripherals)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x09)
			cout << " (Input devices)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0A)
			cout << " (Docking stations)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0B)
			cout << " (Processors)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0C)
			cout << " (Serial bus controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0D)
			cout << " (Wireless controller)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0E)
			cout << " (Intelligent I/O controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x0F)
			cout << " (Satellite communication controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x10)
			cout << " (Encryption/Decryption controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0x11)
			cout << " (Data acquisition and signal processing controllers)" << endl;
		if (stoi(ReverseString(bit.substr(16, 8)), 0, 2) == 0xFF)
			cout << " (Device does not fit in any defined classes)" << endl;
		printf("    -  Cache Line Size : 0x%02X\n", common_configuration_space.cache_line_size);
		printf("    -  Lantency Timer : 0x%02X\n", common_configuration_space.latency_timer);
		printf("   [+] Header Type : 0x%02X\n", common_configuration_space.header_type);
		bit = ReverseString(bitset<8>(common_configuration_space.header_type).to_string());
		cout << "       -  Header Layout (RO) : " << ReverseString(bit.substr(0, 7));
		if (stoi(ReverseString(bit.substr(0, 7)), 0, 2) == 0x00)
			cout << " (Type 0 Configuration Space Header)" << endl;
		if (stoi(ReverseString(bit.substr(0, 7)), 0, 2) == 0x01)
			cout << " (Type 1 Configuration Space Header)" << endl;
		cout << "       -  Multi-Function Device (RO) : " << bit.at(7);
		if (bit.at(7) == '0')
			cout << " (Device is single function)" << endl;
		if (bit.at(7) == '1')
			cout << " (Device has multiple functions)" << endl;
		printf("   [+] BIST : 0x%02X\n", common_configuration_space.bist);
		bit = ReverseString(bitset<8>(common_configuration_space.bist).to_string());
		cout << "       -  Completion Code (RO) : " << ReverseString(bit.substr(0, 4)) << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(4, 2)) << endl;
		cout << "       -  Start BIST (RW) : " << bit.at(6) << endl;
		cout << "       -  BIST Capable (RO) : " << bit.at(7);
		if (bit.at(7) == '0')
			cout << " (Device is not BIST capable)" << endl;
		if (bit.at(7) == '1')
			cout << " (Device supports BIST)" << endl;
		printf("   [+] Base Address Register 0 : 0x%08X\n", type_1_configuration_space_header.base_address_register[0]);
		bit = ReverseString(bitset<32>(type_1_configuration_space_header.base_address_register[0]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		printf("   [+] Base Address Register 1 : 0x%08X\n", type_1_configuration_space_header.base_address_register[1]);
		bit = ReverseString(bitset<32>(type_1_configuration_space_header.base_address_register[1]).to_string());
		cout << "       -  Space Indicator (RO) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (Base Address register is mapped to Memory Space)" << endl;
		if (bit.at(0) == '1')
			cout << " (Base Address register is mapped to I/O Space)" << endl;
		cout << "       -  Type (RO) : " << ReverseString(bit.substr(1, 2));
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x00)
			cout << " (Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x01)
			cout << " (Reserved)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x02)
			cout << " (Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space)" << endl;
		if (stoi(ReverseString(bit.substr(1, 2)), 0, 2) == 0x03)
			cout << " (Reserved)" << endl;
		cout << "       -  Prefetchable (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (The data is not prefetchable)" << endl;
		if (bit.at(3) == '1')
			cout << " (The data is prefetchable)" << endl;
		cout << "       -  Base Address (RW) : " << ReverseString(bit.substr(4, 28)) << endl;
		printf("    -  Primary Bus Number (RW) : 0x%02X\n", type_1_configuration_space_header.primary_bus_number);
		printf("    -  Secondary Bus Number (RW) : 0x%02X\n", type_1_configuration_space_header.secondary_bus_number);
		printf("    -  Subordinate Bus Number (RO) : 0x%02X\n", type_1_configuration_space_header.subordinate_bus_number);
		printf("    -  Secondary Latency Timer (RO) : 0x%02X\n", type_1_configuration_space_header.secondary_latency_timer);
		printf("   [+] I/O Base : 0x%02X\n", type_1_configuration_space_header.io_base);
		bit = ReverseString(bitset<8>(type_1_configuration_space_header.io_base).to_string());
		cout << "       -  I/O Addressing Capability (RO) : " << ReverseString(bit.substr(0, 4));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x00)
			cout << " (16-bit I/O addressing)" << endl;
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			cout << " (32-bit I/O addressing)" << endl;
		cout << "       -  I/O Base Address (RW) : " << ReverseString(bit.substr(4, 4)) << endl;
		printf("   [+] I/O Limit : 0x%02X\n", type_1_configuration_space_header.io_limit);
		bit = ReverseString(bitset<8>(type_1_configuration_space_header.io_limit).to_string());
		cout << "       -  I/O Addressing Capability (RO) : " << ReverseString(bit.substr(0, 4));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x00)
			cout << " (16-bit I/O addressing)" << endl;
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			cout << " (32-bit I/O addressing)" << endl;
		cout << "       -  I/O Limit Address (RW) : " << ReverseString(bit.substr(4, 4)) << endl;
		printf("   [+] Secondary Status : 0x%04X\n", type_1_configuration_space_header.secondary_status);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.secondary_status).to_string());
		cout << "       -  Reserved (RSVDZ) : " << ReverseString(bit.substr(0, 5)) << endl;
		cout << "       -  66 MHz Capable (RO) : " << bit.at(5) << endl;
		cout << "       -  Reserved (RSVDZ) : " << bit.at(6) << endl;
		cout << "       -  Fast Back-to-Back Transactions Capable (RO) : " << bit.at(7) << endl;
		cout << "       -  Master Data Parity Error (RW1C) : " << bit.at(8) << endl;
		cout << "       -  DEVSEL Timing (RO) : " << ReverseString(bit.substr(9, 2)) << endl;
		cout << "       -  Signaled Target Abort (RW1C) : " << bit.at(11) << endl;
		cout << "       -  Received Target Abort (RW1C) : " << bit.at(12) << endl;
		cout << "       -  Received Master Abort (RW1C) : " << bit.at(13) << endl;
		cout << "       -  Received System Error (RW1C) : " << bit.at(14) << endl;
		cout << "       -  Detected Parity Error (RW1C) : " << bit.at(15) << endl;
		printf("   [+] Memory Base : 0x%04X\n", type_1_configuration_space_header.memory_base);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.memory_base).to_string());
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(0, 4)) << endl;
		cout << "       -  Memory Base Address (RW) : " << ReverseString(bit.substr(4, 12)) << endl;
		printf("   [+] Memory Limit : 0x%04X\n", type_1_configuration_space_header.memory_limit);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.memory_limit).to_string());
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(0, 4)) << endl;
		cout << "       -  Memory Limit Address (RW) : " << ReverseString(bit.substr(4, 12)) << endl;
		printf("   [+] Prefetchable Memory Base : 0x%04X\n", type_1_configuration_space_header.prefetchable_memory_base);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.prefetchable_memory_base).to_string());
		cout << "       -  Prefetchable Addressing Capability (RO) : " << ReverseString(bit.substr(0, 4));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x00)
			cout << " (Supports 32-bit addresses)" << endl;
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			cout << " (Supports 64-bit addresses)" << endl;
		cout << "       -  Prefetchable Memory Base Address (RW) : " << ReverseString(bit.substr(4, 12)) << endl;
		printf("   [+] Prefetchable Memory Limit : 0x%04X\n", type_1_configuration_space_header.prefetchable_memory_limit);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.prefetchable_memory_limit).to_string());
		cout << "       -  Prefetchable Addressing Capability (RO) : " << ReverseString(bit.substr(0, 4));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x00)
			cout << " (Supports 32-bit addresses)" << endl;
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			cout << " (Supports 64-bit addresses)" << endl;
		cout << "       -  Prefetchable Memory Limit Address (RW) : " << ReverseString(bit.substr(4, 12)) << endl;
		printf("    -  Prefetchable Base Upper 32 Bits : 0x%08X\n", type_1_configuration_space_header.prefetchable_memory_base_upper);
		printf("    -  Prefetchable Limit Upper 32 Bits : 0x%08X\n", type_1_configuration_space_header.prefetchable_memory_limit_upper);
		printf("    -  I/O Base Upper 16 Bits : 0x%04X\n", type_1_configuration_space_header.io_base_upper);
		printf("    -  I/O Limit Upper 16 Bits : 0x%04X\n", type_1_configuration_space_header.io_limit_upper);
		printf("    -  Capabilities Pointer : 0x%02X\n", common_configuration_space.capabilities_pointer);
		printf("   [+] Expansion ROM Base Address : 0x%08X\n", type_1_configuration_space_header.expansion_rom_base_address);
		bit = ReverseString(bitset<32>(type_1_configuration_space_header.expansion_rom_base_address).to_string());
		cout << "       -  Expansion ROM Enable (RW) : " << bit.at(0);
		if (bit.at(0) == '0')
			cout << " (The device\'s expansion ROM address space is disabled)" << endl;
		if (bit.at(0) == '1')
			cout << " (Address decoding is enabled using the parameters in the other part of the base register)" << endl;
		cout << "       -  Reserved (RSVD) : " << ReverseString(bit.substr(1, 10)) << endl;
		cout << "       -  Expansion ROM Base Address (RW) : " << ReverseString(bit.substr(11, 21)) << endl;
		printf("    -  Interrupt Line : 0x%02X\n", common_configuration_space.interrupt_line);
		printf("    -  Interrupt Pin : 0x%02X\n", common_configuration_space.interrupt_pin);
		printf("   [+] Bridge Control : 0x%04X\n", type_1_configuration_space_header.bridge_control);
		bit = ReverseString(bitset<16>(type_1_configuration_space_header.bridge_control).to_string());
		cout << "       -  Parity Error Response Enable (RW) : " << bit.at(0) << endl;
		cout << "       -  SERR# Enable (RW) : " << bit.at(1) << endl;
		cout << "       -  ISA Enable (RW) : " << bit.at(2) << endl;
		cout << "       -  VGA Enable (RW) : " << bit.at(3) << endl;
		cout << "       -  VGA 16-bit Decode (RW) : " << bit.at(4) << endl;
		cout << "       -  Master Abort Mode (RO) : " << bit.at(5) << endl;
		cout << "       -  Secondary Bus Reset (RW) : " << bit.at(6) << endl;
		cout << "       -  Fast Back-to-Back Transactions Enable (RO) : " << bit.at(7) << endl;
		cout << "       -  Primary Discard Timer (RO) : " << bit.at(8) << endl;
		cout << "       -  Secondary Discard Timer (RO) : " << bit.at(9) << endl;
		cout << "       -  Discard Timer Status (RO) : " << bit.at(10) << endl;
		cout << "       -  Discard Timer SERR# Enable (RO) : " << bit.at(11) << endl;
		cout << "       -  Reserved (RSVDP) : " << ReverseString(bit.substr(12, 4)) << endl << endl;
	}
}

void PrintPciPowerManagementInterfaceCapability(void) {
	string bit;
	if (pci_power_management_interface_capability.exists) {
		CapabilityHighlight("PCI Power Management Capability", pci_power_management_interface_capability.offset);
		ValueHighlight(FALSE, "Capability ID", pci_power_management_interface_capability.capability_id, 2);
		ValueHighlight(FALSE, "Next Capability Pointer", pci_power_management_interface_capability.next_capability_pointer, 2);
		ValueHighlight(TRUE, "Power Management Capabilities", pci_power_management_interface_capability.power_management_capabilities, 4);
		bit = ReverseString(bitset<16>(pci_power_management_interface_capability.power_management_capabilities).to_string());
		cout << "       -  Version (RO) : " << ReverseString(bit.substr(0, 3));
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x01)
			cout << " (PCI Power Management Interface Specification rev.1.0)" << endl;
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x02)
			cout << " (PCI Power Management Interface Specification rev.1.1)" << endl;
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x03)
			cout << " (PCI Power Management Interface Specification rev.1.2)" << endl;
		cout << "       -  PME Clock (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (Indicates that no PCI clock is required for the function to generate PME#)" << endl;
		if (bit.at(3) == '1')
			cout << " (Indicates that the function relies on the presence of the PCI clock for PME# operation)" << endl;
		cout << "       -  Immediate_Readiness_on_Return_to_D0 (RO) : " << bit.at(4) << endl;
		cout << "       -  Device Specific Initialization (RO) : " << bit.at(5) << endl;
		cout << "       -  Aux_Current (RO) : " << ReverseString(bit.substr(6, 3));
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x00)
			cout << " (0mA (self powered))" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x01)
			cout << " (55mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x02)
			cout << " (100mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x03)
			cout << " (160mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x04)
			cout << " (220mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x05)
			cout << " (270mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x06)
			cout << " (320mA)" << endl;
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x07)
			cout << " (375mA)" << endl;
		cout << "       -  D1_Support (RO) : " << bit.at(9) << endl;
		cout << "       -  D2_Support (RO) : " << bit.at(10) << endl;
		cout << "       -  PME_Support (RO) : " << ReverseString(bit.substr(11, 5)) << endl;
		ValueHighlight(TRUE, "Power Management Control/Status", pci_power_management_interface_capability.power_management_control_status, 4);
		bit = ReverseString(bitset<16>(pci_power_management_interface_capability.power_management_control_status).to_string());
		cout << "       -  PowerState (RW) : " << ReverseString(bit.substr(0, 2));
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x00)
			cout << " (D0)" << endl;
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x01)
			cout << " (D1)" << endl;
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x02)
			cout << " (D2)" << endl;
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x03)
			cout << " (D3_Hot)" << endl;
		cout << "       -  Reserved (RSVDP) : " << bit.at(2) << endl;
		cout << "       -  No_Soft_Reset (RO) : " << bit.at(3);
		if (bit.at(3) == '0')
			cout << " (Devices do perform an internal reset upon transitioning from D3hot to D0 via software control of the PowerState bits.)" << endl;
		if (bit.at(3) == '1')
			cout << " (Devices do NOT perform an internal reset upon transitioning from D3hot to D0 via software control of the PowerState bits.)" << endl;
		cout << "       -  Reserved (RSVDP) : " << ReverseString(bit.substr(4, 4)) << endl;
		cout << "       -  PME_En (RW) : " << bit.at(8);
		if (bit.at(8) == '0')
			cout << " (Assertion is disabled)" << endl;
		if (bit.at(8) == '1')
			cout << " (Assertion is enabled)" << endl;
		cout << "       -  Data_Select (RW) : " << ReverseString(bit.substr(9, 4)) << endl;
		cout << "       -  Data_Scale (RO) : " << ReverseString(bit.substr(13, 2)) << endl;
		cout << "       -  No_Soft_Reset (RO) : " << bit.at(15);
		if (bit.at(15) == '0')
			cout << " (Has no effect.)" << endl;
		if (bit.at(15) == '1')
			cout << " (The field will be cleared, and the function will stop asserting a PME# signal (if enabled).)" << endl;
		ValueHighlight(FALSE, "Reserved", pci_power_management_interface_capability.reserved, 2);
		ValueHighlight(FALSE, "Data", pci_power_management_interface_capability.data, 2);
	}
}