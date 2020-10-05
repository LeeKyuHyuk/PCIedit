#include <Windows.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <string>

#include "configuration_space_header.h"
#include "../capability.h"
#include "../pci.h"

using namespace std;

CommonConfigurationSpace common_configuration_space = { 0, };
Type0ConfigurationSpaceHeader type_0_configuration_space_header = { 0, };
Type1ConfigurationSpaceHeader type_1_configuration_space_header = { 0, };

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

void PrintType0ConfigurationSpaceHeader(void) {
	if (type_0_configuration_space_header.exists) {
		CapabilityHighlight("Type 0 Configuration Space Header", 0x00);
		ValueHighlight(FALSE, "Vendor ID", common_configuration_space.vendor_id, 4);
		ValueHighlight(FALSE, "Device ID", common_configuration_space.device_id, 4);
		ValueHighlight(TRUE, "Command", common_configuration_space.command, 4);
		PrintRegisterValue(FALSE, "I/O Space Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 0));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Disables the device response");
			break;
		case 0x01:
			PrintRegisterDescription("Allows the device to respond to I/O Space accesses");
			break;
		}
		PrintRegisterValue(FALSE, "Memory Space Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 1));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 1)) {
		case 0x00:
			PrintRegisterDescription("Disables the device response");
			break;
		case 0x01:
			PrintRegisterDescription("Allows the device to respond to Memory Space accesses");
			break;
		}
		PrintRegisterValue(FALSE, "Bus Master Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 2));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 1)) {
		case 0x00:
			PrintRegisterDescription("PCI Express Function is not allowed to issue any Memory or I/O Requests");
			break;
		case 0x01:
			PrintRegisterDescription("PCI Express Function is allowed to issue Memory or I/O Requests");
			break;
		}
		PrintRegisterValue(TRUE, "Special Cycle Enable", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 3));
		PrintRegisterValue(TRUE, "Memory Write and Invalidate", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 4));
		PrintRegisterValue(TRUE, "VGA Palette Snoop", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 5));
		PrintRegisterValue(TRUE, "Parity Error Response", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 6));
		PrintRegisterValue(TRUE, "IDSEL Stepping/Wait Cycle Control", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 7));
		PrintRegisterValue(TRUE, "SERR# Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 8));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Enable", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 9));
		PrintRegisterValue(TRUE, "Interrupt Disable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 10));
		PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(common_configuration_space.command, 0x1F, 11));
		ValueHighlight(TRUE, "Status", common_configuration_space.status, 4);
		PrintRegisterValue(TRUE, "Immediate Readiness", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 0));
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(common_configuration_space.status, 0x3, 1));
		PrintRegisterValue(TRUE, "Interrupt Status", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 3));
		PrintRegisterValue(TRUE, "Capabilities List", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 4));
		PrintRegisterValue(TRUE, "66 MHz Capable", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 5));
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(common_configuration_space.status, 0x1, 6));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Capable", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 7));
		PrintRegisterValue(TRUE, "Master Data Parity Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 8));
		PrintRegisterValue(TRUE, "DEVSEL Timing", "RO", GetRegisterValue(common_configuration_space.status, 0x3, 9));
		PrintRegisterValue(TRUE, "Signaled Target Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 11));
		PrintRegisterValue(TRUE, "Received Target Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 12));
		PrintRegisterValue(TRUE, "Received Master Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 13));
		PrintRegisterValue(TRUE, "Signaled System Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 14));
		PrintRegisterValue(TRUE, "Detected Parity Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 15));
		ValueHighlight(FALSE, "Revision ID", common_configuration_space.revision_id, 2);
		ValueHighlight(TRUE, "Class Code", common_configuration_space.class_code, 6);
		PrintRegisterValue(TRUE, "Programming Interface", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 0));
		PrintRegisterValue(TRUE, "Sub-Class Code", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 8));
		PrintRegisterValue(FALSE, "Base Class Code", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 16));
		switch (GetRegisterValue(common_configuration_space.class_code, 0xFF, 16)) {
		case 0x00:
			PrintRegisterDescription("Device was built before Class Code definitions were finalized");
			break;
		case 0x01:
			PrintRegisterDescription("Mass storage controller");
			break;
		case 0x02:
			PrintRegisterDescription("Network controller");
			break;
		case 0x03:
			PrintRegisterDescription("Display controller");
			break;
		case 0x04:
			PrintRegisterDescription("Multimedia device");
			break;
		case 0x05:
			PrintRegisterDescription("Memory controller");
			break;
		case 0x06:
			PrintRegisterDescription("Bridge device");
			break;
		case 0x07:
			PrintRegisterDescription("Simple communication controllers");
			break;
		case 0x08:
			PrintRegisterDescription("Base system peripherals");
			break;
		case 0x09:
			PrintRegisterDescription("Input devices");
			break;
		case 0x0A:
			PrintRegisterDescription("Docking stations");
			break;
		case 0x0B:
			PrintRegisterDescription("Processors");
			break;
		case 0x0C:
			PrintRegisterDescription("Serial bus controllers");
			break;
		case 0x0D:
			PrintRegisterDescription("Wireless controller");
			break;
		case 0x0E:
			PrintRegisterDescription("Intelligent I/O controllers");
			break;
		case 0x0F:
			PrintRegisterDescription("Satellite communication controllers");
			break;
		case 0x10:
			PrintRegisterDescription("Encryption/Decryption controllers");
			break;
		case 0x11:
			PrintRegisterDescription("Data acquisition and signal processing controllers");
			break;
		case 0xFF:
			PrintRegisterDescription("Device does not fit in any defined classes");
			break;
		}
		ValueHighlight(FALSE, "Cache Line Size", common_configuration_space.cache_line_size, 2);
		ValueHighlight(FALSE, "Lantency Timer", common_configuration_space.latency_timer, 2);
		ValueHighlight(TRUE, "Header Type", common_configuration_space.header_type, 2);
		PrintRegisterValue(FALSE, "Header Layout", "RO", GetRegisterValue(common_configuration_space.header_type, 0x7F, 0));
		switch (GetRegisterValue(common_configuration_space.header_type, 0x7F, 0)) {
		case 0x00:
			PrintRegisterDescription("Type 0 Configuration Space Header");
			break;
		case 0x01:
			PrintRegisterDescription("Type 1 Configuration Space Header");
			break;
		}
		PrintRegisterValue(FALSE, "Multi-Function Device", "RO", GetRegisterValue(common_configuration_space.header_type, 0x1, 7));
		switch (GetRegisterValue(common_configuration_space.header_type, 0x1, 7)) {
		case 0x00:
			PrintRegisterDescription("Device is single function");
			break;
		case 0x01:
			PrintRegisterDescription("Device has multiple functions");
			break;
		}
		ValueHighlight(TRUE, "BIST", common_configuration_space.bist, 2);
		PrintRegisterValue(TRUE, "Completion Code", "RO", GetRegisterValue(common_configuration_space.bist, 0xF, 0));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(common_configuration_space.bist, 0x3, 4));
		PrintRegisterValue(TRUE, "Start BIST", "RW", GetRegisterValue(common_configuration_space.bist, 0x1, 6));
		PrintRegisterValue(FALSE, "BIST Capable", "RO", GetRegisterValue(common_configuration_space.bist, 0x1, 7));
		switch (GetRegisterValue(common_configuration_space.bist, 0x1, 7)) {
		case 0x00:
			PrintRegisterDescription("Device is not BIST capable");
			break;
		case 0x01:
			PrintRegisterDescription("Device supports BIST");
			break;
		}
		ValueHighlight(TRUE, "Base Address Register 0", type_0_configuration_space_header.base_address_register[0], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x3, 1));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 3));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[0], 0x3FFFFFFF, 2));
		}
		ValueHighlight(FALSE, "Base Address Register 1", type_0_configuration_space_header.base_address_register[1], 8);
		ValueHighlight(TRUE, "Base Address Register 2", type_0_configuration_space_header.base_address_register[2], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x3, 1));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 3));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[2], 0x3FFFFFFF, 2));
		}
		ValueHighlight(TRUE, "Base Address Register 3", type_0_configuration_space_header.base_address_register[3], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x3, 1));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 3));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[3], 0x3FFFFFFF, 2));
		}
		ValueHighlight(TRUE, "Base Address Register 4", type_0_configuration_space_header.base_address_register[4], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x3, 1));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 3));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[4], 0x3FFFFFFF, 2));
		}
		ValueHighlight(TRUE, "Base Address Register 5", type_0_configuration_space_header.base_address_register[5], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x3, 1));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 3));
			switch (GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.base_address_register[5], 0x3FFFFFFF, 2));
		}
		ValueHighlight(FALSE, "Cardbus CIS Pointer", type_0_configuration_space_header.cardbus_cis_pointer, 8);
		ValueHighlight(FALSE, "Subsystem Vendor ID", type_0_configuration_space_header.subsystem_vendor_id, 4);
		ValueHighlight(FALSE, "Subsystem ID", type_0_configuration_space_header.subsystem_id, 4);
		ValueHighlight(TRUE, "Expansion ROM Base Address", type_0_configuration_space_header.expansion_rom_base_address, 8);
		PrintRegisterValue(FALSE, "Expansion ROM Enable", "RW", GetRegisterValue(type_0_configuration_space_header.expansion_rom_base_address, 0x1, 0));
		switch (GetRegisterValue(type_0_configuration_space_header.expansion_rom_base_address, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("The device\'s expansion ROM address space is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Address decoding is enabled using the parameters in the other part of the base register");
			break;
		}
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(type_0_configuration_space_header.expansion_rom_base_address, 0x3FF, 1));
		PrintRegisterValue(TRUE, "Expansion ROM Base Address", "RW", GetRegisterValue(type_0_configuration_space_header.expansion_rom_base_address, 0x1FFFFF, 11));
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
	if (type_1_configuration_space_header.exists) {
		CapabilityHighlight("Type 1 Configuration Space Header", 0x00);
		ValueHighlight(FALSE, "Vendor ID", common_configuration_space.vendor_id, 4);
		ValueHighlight(FALSE, "Device ID", common_configuration_space.device_id, 4);
		ValueHighlight(TRUE, "Command", common_configuration_space.command, 4);
		PrintRegisterValue(FALSE, "I/O Space Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 0));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Disables the device response");
			break;
		case 0x01:
			PrintRegisterDescription("Allows the device to respond to I/O Space accesses");
			break;
		}
		PrintRegisterValue(FALSE, "Memory Space Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 1));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 1)) {
		case 0x00:
			PrintRegisterDescription("Disables the device response");
			break;
		case 0x01:
			PrintRegisterDescription("Allows the device to respond to Memory Space accesses");
			break;
		}
		PrintRegisterValue(FALSE, "Bus Master Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 2));
		switch (GetRegisterValue(common_configuration_space.command, 0x1, 1)) {
		case 0x00:
			PrintRegisterDescription("PCI Express Function is not allowed to issue any Memory or I/O Requests");
			break;
		case 0x01:
			PrintRegisterDescription("PCI Express Function is allowed to issue Memory or I/O Requests");
			break;
		}
		PrintRegisterValue(TRUE, "Special Cycle Enable", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 3));
		PrintRegisterValue(TRUE, "Memory Write and Invalidate", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 4));
		PrintRegisterValue(TRUE, "VGA Palette Snoop", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 5));
		PrintRegisterValue(TRUE, "Parity Error Response", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 6));
		PrintRegisterValue(TRUE, "IDSEL Stepping/Wait Cycle Control", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 7));
		PrintRegisterValue(TRUE, "SERR# Enable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 8));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Enable", "RO", GetRegisterValue(common_configuration_space.command, 0x1, 9));
		PrintRegisterValue(TRUE, "Interrupt Disable", "RW", GetRegisterValue(common_configuration_space.command, 0x1, 10));
		PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(common_configuration_space.command, 0x1F, 11));
		ValueHighlight(TRUE, "Status", common_configuration_space.status, 4);
		PrintRegisterValue(TRUE, "Immediate Readiness", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 0));
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(common_configuration_space.status, 0x3, 1));
		PrintRegisterValue(TRUE, "Interrupt Status", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 3));
		PrintRegisterValue(TRUE, "Capabilities List", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 4));
		PrintRegisterValue(TRUE, "66 MHz Capable", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 5));
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(common_configuration_space.status, 0x1, 6));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Capable", "RO", GetRegisterValue(common_configuration_space.status, 0x1, 7));
		PrintRegisterValue(TRUE, "Master Data Parity Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 8));
		PrintRegisterValue(TRUE, "DEVSEL Timing", "RO", GetRegisterValue(common_configuration_space.status, 0x3, 9));
		PrintRegisterValue(TRUE, "Signaled Target Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 11));
		PrintRegisterValue(TRUE, "Received Target Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 12));
		PrintRegisterValue(TRUE, "Received Master Abort", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 13));
		PrintRegisterValue(TRUE, "Signaled System Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 14));
		PrintRegisterValue(TRUE, "Detected Parity Error", "RW1C", GetRegisterValue(common_configuration_space.status, 0x1, 15));
		ValueHighlight(FALSE, "Revision ID", common_configuration_space.revision_id, 2);
		ValueHighlight(TRUE, "Class Code", common_configuration_space.class_code, 6);
		PrintRegisterValue(TRUE, "Programming Interface", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 0));
		PrintRegisterValue(TRUE, "Sub-Class Code", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 8));
		PrintRegisterValue(FALSE, "Base Class Code", "RO", GetRegisterValue(common_configuration_space.class_code, 0xFF, 16));
		switch (GetRegisterValue(common_configuration_space.class_code, 0xFF, 16)) {
		case 0x00:
			PrintRegisterDescription("Device was built before Class Code definitions were finalized");
			break;
		case 0x01:
			PrintRegisterDescription("Mass storage controller");
			break;
		case 0x02:
			PrintRegisterDescription("Network controller");
			break;
		case 0x03:
			PrintRegisterDescription("Display controller");
			break;
		case 0x04:
			PrintRegisterDescription("Multimedia device");
			break;
		case 0x05:
			PrintRegisterDescription("Memory controller");
			break;
		case 0x06:
			PrintRegisterDescription("Bridge device");
			break;
		case 0x07:
			PrintRegisterDescription("Simple communication controllers");
			break;
		case 0x08:
			PrintRegisterDescription("Base system peripherals");
			break;
		case 0x09:
			PrintRegisterDescription("Input devices");
			break;
		case 0x0A:
			PrintRegisterDescription("Docking stations");
			break;
		case 0x0B:
			PrintRegisterDescription("Processors");
			break;
		case 0x0C:
			PrintRegisterDescription("Serial bus controllers");
			break;
		case 0x0D:
			PrintRegisterDescription("Wireless controller");
			break;
		case 0x0E:
			PrintRegisterDescription("Intelligent I/O controllers");
			break;
		case 0x0F:
			PrintRegisterDescription("Satellite communication controllers");
			break;
		case 0x10:
			PrintRegisterDescription("Encryption/Decryption controllers");
			break;
		case 0x11:
			PrintRegisterDescription("Data acquisition and signal processing controllers");
			break;
		case 0xFF:
			PrintRegisterDescription("Device does not fit in any defined classes");
			break;
		}
		ValueHighlight(FALSE, "Cache Line Size", common_configuration_space.cache_line_size, 2);
		ValueHighlight(FALSE, "Lantency Timer", common_configuration_space.latency_timer, 2);
		ValueHighlight(TRUE, "Header Type", common_configuration_space.header_type, 2);
		PrintRegisterValue(FALSE, "Header Layout", "RO", GetRegisterValue(common_configuration_space.header_type, 0x7F, 0));
		switch (GetRegisterValue(common_configuration_space.header_type, 0x7F, 0)) {
		case 0x00:
			PrintRegisterDescription("Type 0 Configuration Space Header");
			break;
		case 0x01:
			PrintRegisterDescription("Type 1 Configuration Space Header");
			break;
		}
		PrintRegisterValue(FALSE, "Multi-Function Device", "RO", GetRegisterValue(common_configuration_space.header_type, 0x1, 7));
		switch (GetRegisterValue(common_configuration_space.header_type, 0x1, 7)) {
		case 0x00:
			PrintRegisterDescription("Device is single function");
			break;
		case 0x01:
			PrintRegisterDescription("Device has multiple functions");
			break;
		}
		ValueHighlight(TRUE, "BIST", common_configuration_space.bist, 2);
		PrintRegisterValue(TRUE, "Completion Code", "RO", GetRegisterValue(common_configuration_space.bist, 0xF, 0));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(common_configuration_space.bist, 0x3, 4));
		PrintRegisterValue(TRUE, "Start BIST", "RW", GetRegisterValue(common_configuration_space.bist, 0x1, 6));
		PrintRegisterValue(FALSE, "BIST Capable", "RO", GetRegisterValue(common_configuration_space.bist, 0x1, 7));
		switch (GetRegisterValue(common_configuration_space.bist, 0x1, 7)) {
		case 0x00:
			PrintRegisterDescription("Device is not BIST capable");
			break;
		case 0x01:
			PrintRegisterDescription("Device supports BIST");
			break;
		}
		ValueHighlight(TRUE, "Base Address Register 0", type_1_configuration_space_header.base_address_register[0], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x3, 1));
			switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 3));
			switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.base_address_register[0], 0x3FFFFFFF, 2));
		}
		ValueHighlight(TRUE, "Base Address Register 1", type_1_configuration_space_header.base_address_register[1], 8);
		PrintRegisterValue(FALSE, "Space Indicator", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Base Address register is mapped to Memory Space");
			break;
		case 0x01:
			PrintRegisterDescription("Base Address register is mapped to I/O Space");
			break;
		}
		if (GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 0) == 0x00) {
			/* Base Address Register for Memory */
			PrintRegisterValue(FALSE, "Type", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x3, 1));
			switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x3, 1)) {
			case 0x00:
				PrintRegisterDescription("Base register is 32 bits wide and can be mapped anywhere in the 32 address bit Memory Space");
				break;
			case 0x01:
				PrintRegisterDescription("Reserved");
				break;
			case 0x02:
				PrintRegisterDescription("Base register is 64 bits wide and can be mapped anywhere in the 64 address bit Memory Space");
				break;
			case 0x03:
				PrintRegisterDescription("Reserved");
				break;
			}
			PrintRegisterValue(FALSE, "Prefetchable", "RO", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 3));
			switch (GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 3)) {
			case 0x00:
				PrintRegisterDescription("The data is not prefetchable");
				break;
			case 0x01:
				PrintRegisterDescription("The data is prefetchable");
				break;
			}
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x0FFFFFFF, 4));
		}
		if (GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 0) == 0x01) {
			/* Base Address Register for I/O */
			PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x1, 1));
			PrintRegisterValue(TRUE, "Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.base_address_register[1], 0x3FFFFFFF, 2));
		}
		ValueHighlight(FALSE, "Primary Bus Number", type_1_configuration_space_header.primary_bus_number, 2);
		ValueHighlight(FALSE, "Secondary Bus Number", type_1_configuration_space_header.secondary_bus_number, 2);
		ValueHighlight(FALSE, "Subordinate Bus Number", type_1_configuration_space_header.subordinate_bus_number, 2);
		ValueHighlight(FALSE, "Secondary Latency Timer", type_1_configuration_space_header.secondary_latency_timer, 2);
		ValueHighlight(TRUE, "I/O Base", type_1_configuration_space_header.io_base, 2);
		PrintRegisterValue(FALSE, "I/O Addressing Capability", "RO", GetRegisterValue(type_1_configuration_space_header.io_base, 0xF, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.io_base, 0xF, 0)) {
		case 0x00:
			PrintRegisterDescription("16-bit I/O addressing");
			break;
		case 0x01:
			PrintRegisterDescription("32-bit I/O addressing");
			break;
		}
		PrintRegisterValue(TRUE, "I/O Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.io_base, 0xF, 4));
		ValueHighlight(TRUE, "I/O Limit", type_1_configuration_space_header.io_limit, 2);
		PrintRegisterValue(FALSE, "I/O Addressing Capability", "RO", GetRegisterValue(type_1_configuration_space_header.io_limit, 0xF, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.io_limit, 0xF, 0)) {
		case 0x00:
			PrintRegisterDescription("16-bit I/O addressing");
			break;
		case 0x01:
			PrintRegisterDescription("32-bit I/O addressing");
			break;
		}
		PrintRegisterValue(TRUE, "I/O Limit Address", "RW", GetRegisterValue(type_1_configuration_space_header.io_limit, 0xF, 4));
		ValueHighlight(TRUE, "Secondary Status", type_1_configuration_space_header.secondary_status, 4);
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1F, 0));
		PrintRegisterValue(TRUE, "66 MHz Capable", "RO", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 5));
		PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 6));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Capable", "RO", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 7));
		PrintRegisterValue(TRUE, "Master Data Parity Error", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 8));
		PrintRegisterValue(TRUE, "DEVSEL Timing", "RO", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x3, 9));
		PrintRegisterValue(TRUE, "Signaled Target Abort", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 11));
		PrintRegisterValue(TRUE, "Received Target Abort", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 12));
		PrintRegisterValue(TRUE, "Received Master Abort", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 13));
		PrintRegisterValue(TRUE, "Received System Error", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 14));
		PrintRegisterValue(TRUE, "Detected Parity Error", "RW1C", GetRegisterValue(type_1_configuration_space_header.secondary_status, 0x1, 15));
		ValueHighlight(TRUE, "Memory Base", type_1_configuration_space_header.memory_base, 4);
		PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_1_configuration_space_header.memory_base, 0xF, 0));
		PrintRegisterValue(TRUE, "Memory Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.memory_base, 0xFFF, 4));
		ValueHighlight(TRUE, "Memory Limit", type_1_configuration_space_header.memory_limit, 4);
		PrintRegisterValue(TRUE, "Reserved", "RSVD", GetRegisterValue(type_1_configuration_space_header.memory_limit, 0xF, 0));
		PrintRegisterValue(TRUE, "Memory Limit Address", "RW", GetRegisterValue(type_1_configuration_space_header.memory_limit, 0xFFF, 4));
		ValueHighlight(TRUE, "Prefetchable Memory Base", type_1_configuration_space_header.prefetchable_memory_base, 4);
		PrintRegisterValue(FALSE, "Prefetchable Addressing Capability", "RO", GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_base, 0xF, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_base, 0xF, 0)) {
		case 0x00:
			PrintRegisterDescription("Supports 32-bit addresses");
			break;
		case 0x01:
			PrintRegisterDescription("Supports 64-bit addresses");
			break;
		}
		PrintRegisterValue(TRUE, "Prefetchable Memory Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_base, 0xFFF, 4));
		ValueHighlight(TRUE, "Prefetchable Memory Limit", type_1_configuration_space_header.prefetchable_memory_limit, 4);
		PrintRegisterValue(FALSE, "Prefetchable Addressing Capability", "RO", GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_limit, 0xF, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_limit, 0xF, 0)) {
		case 0x00:
			PrintRegisterDescription("Supports 32-bit addresses");
			break;
		case 0x01:
			PrintRegisterDescription("Supports 64-bit addresses");
			break;
		}
		PrintRegisterValue(TRUE, "Prefetchable Memory Limit Address", "RW", GetRegisterValue(type_1_configuration_space_header.prefetchable_memory_limit, 0xFFF, 4));
		ValueHighlight(FALSE, "Prefetchable Base Upper 32 Bits", type_1_configuration_space_header.prefetchable_memory_base_upper, 8);
		ValueHighlight(FALSE, "Prefetchable Limit Upper 32 Bits", type_1_configuration_space_header.prefetchable_memory_limit_upper, 8);
		ValueHighlight(FALSE, "I/O Base Upper 16 Bits", type_1_configuration_space_header.io_base_upper, 4);
		ValueHighlight(FALSE, "I/O Limit Upper 16 Bits", type_1_configuration_space_header.io_limit_upper, 4);
		ValueHighlight(FALSE, "Capabilities Pointer", common_configuration_space.capabilities_pointer, 2);
		ValueHighlight(TRUE, "Expansion ROM Base Address", type_1_configuration_space_header.expansion_rom_base_address, 8);
		PrintRegisterValue(FALSE, "Expansion ROM Enable", "RW", GetRegisterValue(type_1_configuration_space_header.expansion_rom_base_address, 0x1, 0));
		switch (GetRegisterValue(type_1_configuration_space_header.expansion_rom_base_address, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("The device\'s expansion ROM address space is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Address decoding is enabled using the parameters in the other part of the base register");
			break;
		}
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(type_1_configuration_space_header.expansion_rom_base_address, 0x3FF, 1));
		PrintRegisterValue(TRUE, "Expansion ROM Base Address", "RW", GetRegisterValue(type_1_configuration_space_header.expansion_rom_base_address, 0x1FFFFF, 11));
		ValueHighlight(FALSE, "Interrupt Line", common_configuration_space.interrupt_line, 2);
		ValueHighlight(FALSE, "Interrupt Pin", common_configuration_space.interrupt_pin, 2);
		ValueHighlight(TRUE, "Bridge Control", type_1_configuration_space_header.bridge_control, 4);
		PrintRegisterValue(TRUE, "Parity Error Response Enable", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 0));
		PrintRegisterValue(TRUE, "SERR# Enable", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 1));
		PrintRegisterValue(TRUE, "ISA Enable", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 2));
		PrintRegisterValue(TRUE, "VGA Enable", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 3));
		PrintRegisterValue(TRUE, "VGA 16-bit Decode", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 4));
		PrintRegisterValue(TRUE, "Master Abort Mode", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 5));
		PrintRegisterValue(TRUE, "Secondary Bus Reset", "RW", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 6));
		PrintRegisterValue(TRUE, "Fast Back-to-Back Transactions Enable", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 7));
		PrintRegisterValue(TRUE, "Primary Discard Timer", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 8));
		PrintRegisterValue(TRUE, "Secondary Discard Timer", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 9));
		PrintRegisterValue(TRUE, "Discard Timer Status", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 10));
		PrintRegisterValue(TRUE, "Discard Timer SERR# Enable", "RO", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0x1, 11));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(type_1_configuration_space_header.bridge_control, 0xF, 12));
	}
}