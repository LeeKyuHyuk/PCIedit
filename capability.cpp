#include <Windows.h>
#include <iostream>

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
	WORD uo_base_upper;
	WORD io_base_limit;
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

CommonConfigurationSpace common_configuration_space = { 0, };
Type0ConfigurationSpaceHeader type_0_configuration_space_header = { 0, };
Type1ConfigurationSpaceHeader type_1_configuration_space_header = { 0, };
PciPowerManagementInterfaceCapability pci_power_management_interface_capability = { 0, };

void PrintRegisters(int bus, int device, int function) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                 [00h] Type 0 Configuration Space Header               |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|              Device ID            |             Vendor ID             |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 0, &data);
	printf("|                0x%04X             |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------|-----------------------------------|" << endl;
	cout << "|               Status              |              Command              |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 1, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------|-----------------------------------|" << endl;
	cout << "|             Class Code            |             Revision ID           |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 2, &data);
	printf("|              0x%06X             |                0x%02X               |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|    BIST    |   Header Type    |   Lantency Timer  |  Cache Line Size  |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 3, &data);
	printf("|    0x%02X    |       0x%02X       |        0x%02X       |        0x%02X       |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 0                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 4, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 1                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 5, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 2                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 6, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 3                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 7, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 4                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 8, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 5                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 9, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                           Cardbus CIS Pointer                         |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 10, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|            Subsystem ID           |         Subsystem Vendor ID       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 11, &data);
	printf("|                0x%04X             |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                        Expansion ROM Base Address                     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 12, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|              Reserved             |        Capabilities Pointer       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 13, &data);
	printf("|              0x%06X             |                0x%02X               |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                                Reserved                               |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 14, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|   Max Latency   |   Min Grant   |   Interrupt Pin  |  Interrupt Line  |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 15, &data);
	printf("|       0x%02X      |      0x%02X     |        0x%02X      |       0x%02X       |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                 [40h] PCI Power Management Capability                 |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|   Power Management Capabilities   | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 16, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|         Data         |    Power Management Control/Status Register    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 17, &data);
	printf("|         0x%02X         |                    0x%06X                    |\n", HIBYTE(HIWORD(data[1])), LOWORD(data[1]) << 8 >> 8);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          [50h] MSI Capability                         |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|          Message Control          | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 20, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Message Lower Address                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 21, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Message Upper Address                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 22, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                                   |            Message Data           |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 23, &data);
	printf("|                                   |               0x%04X              |\n", LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         [70h] MSI-X Capability                        |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|          Message Control          | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 28, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Table Offset/Table BIR                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 29, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                           PBA Offset/PBA BIR                          |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 30, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      [80h] PCI Express Capability                     |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "| PCI Express Capabilities Register | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 32, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      Device Capabilities Register                     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 33, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|       Device Status Register      |      Device Control Register      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 34, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                       Link Capabilities Register                      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 35, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|        Link Status Register       |       Link Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 36, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                       Slot Capabilities Register                      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 37, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|        Slot Status Register       |       Slot Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 38, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|     Root Capabilities Register    |       Root Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 39, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Root Status Register                         |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 40, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                     Device Capabilities 2 Register                    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 41, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|      Device Status 2 Register     |     Device Control 2 Register     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 42, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      Link Capabilities 2 Register                     |" << endl;
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|       Link Status 2 Register      |      Link Control 2 Register      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 44, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl << endl;

}

void GetStandardCapabilities(int bus, int device, int function) {
	GetCommonConfigurationSpace(bus, device, function);
	if (common_configuration_space.header_type == 0)
		GetType0ConfigurationSpaceHeader(bus, device, function);
	if (common_configuration_space.header_type == 1)
		GetType1ConfigurationSpaceHeader(bus, device, function);
	BYTE next_item_pointer = common_configuration_space.capabilities_pointer;
	while (TRUE) {
		BYTE capability_id = GetCapabilityID(bus, device, function, next_item_pointer);
		printf("[DEBUG] Capability ID : 0x%02X\n", capability_id);
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
		printf("[DEBUG] Next Item Pointer : 0x%02X\n", next_item_pointer);
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
	type_1_configuration_space_header.uo_base_upper = LOWORD(data[1]);
	type_1_configuration_space_header.io_base_limit = HIWORD(data[1]);
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

void PrintType0ConfigurationSpaceHeader(void) {
	if (type_0_configuration_space_header.exists) {
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                [000h] Type 0 Configuration Space Header               |" << endl;
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|              Device ID            |             Vendor ID             |" << endl;
		printf("|                0x%04X             |               0x%04X              |\n", common_configuration_space.device_id, common_configuration_space.vendor_id);
		cout << "|-----------------------------------|-----------------------------------|" << endl;
		cout << "|               Status              |              Command              |" << endl;
		printf("|               0x%04X              |               0x%04X              |\n", common_configuration_space.status, common_configuration_space.command);
		cout << "|-----------------------------------|-----------------------------------|" << endl;
		cout << "|             Class Code            |             Revision ID           |" << endl;
		printf("|              0x%06X             |                0x%02X               |\n", common_configuration_space.class_code, common_configuration_space.revision_id);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|    BIST    |   Header Type    |   Lantency Timer  |  Cache Line Size  |" << endl;
		printf("|    0x%02X    |       0x%02X       |        0x%02X       |        0x%02X       |\n", common_configuration_space.bist, common_configuration_space.header_type, common_configuration_space.latency_timer, common_configuration_space.cache_line_size);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 0                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[0]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 1                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[1]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 2                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[2]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 3                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[3]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 4                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[4]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 5                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[5]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                           Cardbus CIS Pointer                         |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.cardbus_cis_pointer);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|            Subsystem ID           |         Subsystem Vendor ID       |" << endl;
		printf("|                0x%04X             |               0x%04X              |\n", type_0_configuration_space_header.subsystem_id, type_0_configuration_space_header.subsystem_vendor_id);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                        Expansion ROM Base Address                     |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.expansion_rom_base_address);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|              Reserved             |        Capabilities Pointer       |" << endl;
		printf("|              0x%06X             |                0x%02X               |\n", type_0_configuration_space_header.reserved1, common_configuration_space.capabilities_pointer);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                                Reserved                               |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.reserved2);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|   Max Latency   |   Min Grant   |   Interrupt Pin  |  Interrupt Line  |" << endl;
		printf("|       0x%02X      |      0x%02X     |        0x%02X      |       0x%02X       |\n", type_0_configuration_space_header.max_latency, type_0_configuration_space_header.min_grant, common_configuration_space.interrupt_pin, common_configuration_space.interrupt_line);
		cout << "|-----------------------------------------------------------------------|" << endl << endl;
	}
}

void PrintPciPowerManagementInterfaceCapability(void) {
	if (pci_power_management_interface_capability.exists) {
		cout << "|-----------------------------------------------------------------------|" << endl;
		printf("|                [%03Xh] PCI Power Management Capability                 |\n", pci_power_management_interface_capability.offset);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|Power Management Capabilities| Next Capability Pointer | Capability ID |" << endl;
		printf("|            0x%04X           |           0x%02X          |      0x%02X     |\n", pci_power_management_interface_capability.power_management_capabilities, pci_power_management_interface_capability.next_capability_pointer, pci_power_management_interface_capability.capability_id);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|   Data   |   Reserved   |   Power Management Control/Status Register  |" << endl;
		printf("|   0x%02X   |     0x%02X     |                   0x%06X                  |\n", pci_power_management_interface_capability.data, pci_power_management_interface_capability.reserved, pci_power_management_interface_capability.power_management_control_status);
		cout << "|-----------------------------------------------------------------------|" << endl;
	}
}