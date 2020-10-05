#include <Windows.h>
#include <bitset>
#include <string>

#include "pci_express.h"
#include "configuration_space_header.h"
#include "../capability.h"
#include "../pci.h"

using namespace std;

extern Type0ConfigurationSpaceHeader type_0_configuration_space_header;
extern Type1ConfigurationSpaceHeader type_1_configuration_space_header;

PciExpressCapability pci_express_capability = { 0, };

void GetPciExpressCapability(int bus, int device, int function, BYTE offset) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	pci_express_capability.offset = offset;
	ReadPciDword(bus, device, function, offset + 0x00, &data);
	pci_express_capability.capability_id = LOBYTE(data[1]);
	pci_express_capability.next_capability_pointer = HIBYTE(LOWORD(data[1]));
	pci_express_capability.pci_express_capabilities = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x04, &data);
	pci_express_capability.device_capabilities = data[1];
	ReadPciDword(bus, device, function, offset + 0x08, &data);
	pci_express_capability.device_control = LOWORD(data[1]);
	pci_express_capability.device_status = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x0C, &data);
	pci_express_capability.link_capabilities = data[1];
	ReadPciDword(bus, device, function, offset + 0x10, &data);
	pci_express_capability.link_control = LOWORD(data[1]);
	pci_express_capability.link_status = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x14, &data);
	pci_express_capability.slot_capabilities = data[1];
	ReadPciDword(bus, device, function, offset + 0x18, &data);
	pci_express_capability.slot_control = LOWORD(data[1]);
	pci_express_capability.slot_status = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x1C, &data);
	pci_express_capability.root_control = LOWORD(data[1]);
	pci_express_capability.root_capabilities = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x20, &data);
	pci_express_capability.root_status = data[1];
	ReadPciDword(bus, device, function, offset + 0x24, &data);
	pci_express_capability.device_capabilities_2 = data[1];
	ReadPciDword(bus, device, function, offset + 0x28, &data);
	pci_express_capability.device_control_2 = LOWORD(data[1]);
	pci_express_capability.device_status_2 = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x2C, &data);
	pci_express_capability.link_capabilities_2 = data[1];
	ReadPciDword(bus, device, function, offset + 0x30, &data);
	pci_express_capability.link_control_2 = LOWORD(data[1]);
	pci_express_capability.link_status_2 = HIWORD(data[1]);
	ReadPciDword(bus, device, function, offset + 0x34, &data);
	pci_express_capability.slot_capabilities_2 = data[1];
	ReadPciDword(bus, device, function, offset + 0x38, &data);
	pci_express_capability.slot_control_2 = LOWORD(data[1]);
	pci_express_capability.slot_status_2 = HIWORD(data[1]);
	pci_express_capability.exists = TRUE;
}

void PrintPciExpressCapability(void) {
	/*
	string bit;
	if (pci_express_capability.exists) {
		CapabilityHighlight("PCI Express Capability", pci_express_capability.offset);
		ValueHighlight(FALSE, "Capability ID", pci_express_capability.capability_id, 2);
		ValueHighlight(FALSE, "Next Capability Pointer", pci_express_capability.next_capability_pointer, 2);
		ValueHighlight(TRUE, "PCI Express Capabilities", pci_express_capability.pci_express_capabilities, 4);
		bit = ReverseString(bitset<16>(pci_express_capability.pci_express_capabilities).to_string());
		PrintRegisterValue(FALSE, "Capability Version", "RO", ReverseString(bit.substr(0, 4)));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			PrintRegisterDescription("Version 1 (Compliant with PCI Express Base Specification rev 1.0)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x02)
			PrintRegisterDescription("Version 2 (Compliant with PCI Express Base Specification rev 2.0)");
		PrintRegisterValue(FALSE, "Device/Port Type", "RO", ReverseString(bit.substr(4, 4)));
		if (type_0_configuration_space_header.exists) {
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x00)
				PrintRegisterDescription("PCI Express Endpoint");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x01)
				PrintRegisterDescription("Legacy PCI Express Endpoint");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x09)
				PrintRegisterDescription("Root Complex Integrated Endpoint");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x0A)
				PrintRegisterDescription("Root Complex Event Collector");
		}
		if (type_1_configuration_space_header.exists) {
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x04)
				PrintRegisterDescription("Root Port of PCI Express Root Complex");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x05)
				PrintRegisterDescription("Upstream Port of PCI Express Switch");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x06)
				PrintRegisterDescription("Downstream Port of PCI Express Switch");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x07)
				PrintRegisterDescription("PCI Express to PCI/PCI-X Bridge");
			if (stoi(ReverseString(bit.substr(4, 4)), 0, 2) == 0x08)
				PrintRegisterDescription("PCI/PCI-X to PCI Express Bridge");
		}
		PrintRegisterValue(FALSE, "Slot Implemented", "HWINIT", bit.at(8));
		if (bit.at(8) == '0')
			PrintRegisterDescription("Link is NOT connected to a slot");
		if (bit.at(8) == '1')
			PrintRegisterDescription("Link is connected to a slot");
		PrintRegisterValue(TRUE, "Interrupt Message Number", "RO", ReverseString(bit.substr(9, 5)));
		PrintRegisterValue(TRUE, "Undefined", "RO", bit.at(14));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", bit.at(15));
		ValueHighlight(FALSE, "Device Capabilities", pci_express_capability.device_capabilities, 8);
		bit = ReverseString(bitset<32>(pci_express_capability.device_capabilities).to_string());
		PrintRegisterValue(FALSE, "Max_Payload_Size Supported", "RO", ReverseString(bit.substr(0, 3)));
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("128 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("256 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("512 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("1024 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("2048 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("4096 bytes max payload size");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("Reserved");
		if (stoi(ReverseString(bit.substr(0, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("Reserved");
		PrintRegisterValue(FALSE, "Phantom Functions Supported", "RO", ReverseString(bit.substr(3, 2)));
		if (stoi(ReverseString(bit.substr(3, 2)), 0, 2) == 0x00)
			PrintRegisterDescription("No Function Number bits are used for Phantom Functions. Multi-Function Devices are permitted to implement up to 8 independent Functions.");
		if (stoi(ReverseString(bit.substr(3, 2)), 0, 2) == 0x01)
			PrintRegisterDescription("The most significant bit of the Function number in Requester ID is used for Phantom Functions; a Multi-Function Device is permitted to implement Functions 0-3. Functions 0, 1, 2, and 3 are permitted to use Function Numbers 4, 5, 6, and 7 respectively as Phantom Functions.");
		if (stoi(ReverseString(bit.substr(3, 2)), 0, 2) == 0x02)
			PrintRegisterDescription("The two most significant bits of Function Number in Requester ID are used for Phantom Functions; a Multi-Function Device is permitted to implement Functions 0-1. Function 0 is permitted to use Function Numbers 2, 4, and 6 for Phantom Functions. Function 1 is permitted to use Function Numbers 3, 5, and 7 as Phantom Functions. ");
		if (stoi(ReverseString(bit.substr(3, 2)), 0, 2) == 0x03)
			PrintRegisterDescription("All 3 bits of Function Number in Requester ID used for Phantom Functions. The device must have a single Function 0 that is permitted to use all other Function Numbers as Phantom Functions.");
		PrintRegisterValue(FALSE, "Extended Tag Field Supported", "RO", bit.at(5));
		if (bit.at(5) == '0')
			PrintRegisterDescription("5-bit Tag field supported");
		if (bit.at(5) == '1')
			PrintRegisterDescription("8-bit Tag field supported");
		PrintRegisterValue(FALSE, "Endpoint L0s Acceptable Latency", "RO", ReverseString(bit.substr(6, 3)));
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("Maximum of 64 ns");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("Maximum of 128 ns");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("Maximum of 256 ns");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("Maximum of 512 ns");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("Maximum of 1 レs");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("Maximum of 2 レs");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("Maximum of 4 レs");
		if (stoi(ReverseString(bit.substr(6, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("No limit");
		PrintRegisterValue(FALSE, "Endpoint L1 Acceptable Latency", "RO", ReverseString(bit.substr(9, 3)));
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("Maximum of 1 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("Maximum of 2 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("Maximum of 4 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("Maximum of 8 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("Maximum of 16 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("Maximum of 32 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("Maximum of 64 レs");
		if (stoi(ReverseString(bit.substr(9, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("No limit");
		PrintRegisterValue(TRUE, "Undefined", "RO", ReverseString(bit.substr(12, 3)));
		PrintRegisterValue(FALSE, "Role-Based Error Reporting", "RO", bit.at(15));
		if (bit.at(15) == '0')
			PrintRegisterDescription("Function does NOT implement the functionality defined in the Error Reporting ECN");
		if (bit.at(15) == '1')
			PrintRegisterDescription("Function implements the functionality defined in the Error Reporting ECN");
		PrintRegisterValue(TRUE, "ERR_COR Subclass Capable", "RO", bit.at(16));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", bit.at(17));
		PrintRegisterValue(TRUE, "Captured Slot Power Limit Value", "RO", ReverseString(bit.substr(18, 8)));
		PrintRegisterValue(FALSE, "Captured Slot Power Limit Scale", "RO", ReverseString(bit.substr(26, 2)));
		if (stoi(ReverseString(bit.substr(26, 2)), 0, 2) == 0x00)
			PrintRegisterDescription("1.0x");
		if (stoi(ReverseString(bit.substr(26, 2)), 0, 2) == 0x01)
			PrintRegisterDescription("0.1x");
		if (stoi(ReverseString(bit.substr(26, 2)), 0, 2) == 0x02)
			PrintRegisterDescription("0.01x");
		if (stoi(ReverseString(bit.substr(26, 2)), 0, 2) == 0x03)
			PrintRegisterDescription("0.001x");
		PrintRegisterValue(FALSE, "Function Level Reset Capability", "RO", bit.at(28));
		if (bit.at(28) == '0')
			PrintRegisterDescription("Function does NOT support the Function Level Reset mechanism");
		if (bit.at(28) == '1')
			PrintRegisterDescription("Function supports the Function Level Reset mechanism");
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", ReverseString(bit.substr(29, 3)));
		ValueHighlight(FALSE, "Device Control", pci_express_capability.device_control, 4);
		bit = ReverseString(bitset<16>(pci_express_capability.device_control).to_string());
		PrintRegisterValue(FALSE, "Correctable Error Reporting Enable", "RW", bit.at(0));
		if (bit.at(0) == '0')
			PrintRegisterDescription("Sending ERR_COR Message is disabled");
		if (bit.at(0) == '1')
			PrintRegisterDescription("Sending ERR_COR Message is enabled");
		PrintRegisterValue(FALSE, "Non-Fatal Error Reporting Enable", "RW", bit.at(1));
		if (bit.at(1) == '0')
			PrintRegisterDescription("Sending ERR_NONFATAL Message is disabled");
		if (bit.at(1) == '1')
			PrintRegisterDescription("Sending ERR_NONFATAL Message is enabled");
		PrintRegisterValue(FALSE, "Fatal Error Reporting Enable", "RW", bit.at(2));
		if (bit.at(2) == '0')
			PrintRegisterDescription("Sending ERR_FATAL Message is disabled");
		if (bit.at(2) == '1')
			PrintRegisterDescription("Sending ERR_FATAL Message is enabled");
		PrintRegisterValue(FALSE, "Unsupported Request Reporting Enable", "RW", bit.at(3));
		if (bit.at(3) == '0')
			PrintRegisterDescription("Signaling of Unsupported Request Errors is disabled");
		if (bit.at(3) == '1')
			PrintRegisterDescription("Signaling of Unsupported Request Errors is enabled");
		PrintRegisterValue(FALSE, "Enable Relaxed Ordering", "RW", bit.at(4));
		if (bit.at(4) == '0')
			PrintRegisterDescription("Function is NOT permitted to set the Relaxed Ordering bit");
		if (bit.at(4) == '1')
			PrintRegisterDescription("Function is permitted to set the Relaxed Ordering bit");
		PrintRegisterValue(FALSE, "Max_Payload_Size", "RW", ReverseString(bit.substr(5, 3)));
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("128 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("256 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("512 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("1024 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("2048 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("4096 bytes max payload size");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("Reserved");
		if (stoi(ReverseString(bit.substr(5, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("Reserved");
		PrintRegisterValue(FALSE, "Extended Tag Field Enable", "RW", bit.at(8));
		if (bit.at(8) == '0')
			PrintRegisterDescription("The Function is restricted to a 5-bit Tag field");
		if (bit.at(8) == '1')
			PrintRegisterDescription("Enables the Function to use an 8-bit Tag field");
		PrintRegisterValue(FALSE, "Phantom Functions Enable", "RW", bit.at(9));
		if (bit.at(9) == '0')
			PrintRegisterDescription("The Function is NOT allowed to use Phantom Functions");
		if (bit.at(9) == '1')
			PrintRegisterDescription("The Function is allowed to use Phantom Functions");
		PrintRegisterValue(FALSE, "Aux Power PM Enable", "RWS", bit.at(10));
		if (bit.at(10) == '0')
			PrintRegisterDescription("The Function is NOT enabled allowed to draw Aux power");
		if (bit.at(10) == '1')
			PrintRegisterDescription("The Function is enabled allowed to draw Aux power");
		PrintRegisterValue(FALSE, "Enable No Snoop", "RW", bit.at(11));
		if (bit.at(11) == '0')
			PrintRegisterDescription("The Function is NOT permitted to set the No Snoop bit");
		if (bit.at(11) == '1')
			PrintRegisterDescription("The Function is permitted to set the No Snoop bit");
		PrintRegisterValue(FALSE, "Max_Read_Request_Size", "RW", ReverseString(bit.substr(12, 3)));
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("128 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("256 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("512 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("1024 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("2048 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("4096 bytes max payload size");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("Reserved");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("Reserved");
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", bit.at(15));
		ValueHighlight(FALSE, "Device Status", pci_express_capability.device_status, 4);
		bit = ReverseString(bitset<16>(pci_express_capability.device_status).to_string());
		PrintRegisterValue(FALSE, "Correctable Error Detected", "RW1C", bit.at(0));
		if (bit.at(0) == '0')
			PrintRegisterDescription("Correctable errors NOT detected");
		if (bit.at(0) == '1')
			PrintRegisterDescription("Correctable errors detected");
		PrintRegisterValue(FALSE, "Non-Fatal Error Detected", "RW1C", bit.at(1));
		if (bit.at(1) == '0')
			PrintRegisterDescription("Non-fatal errors NOT detected");
		if (bit.at(1) == '1')
			PrintRegisterDescription("Non-fatal errors detected");
		PrintRegisterValue(FALSE, "Fatal Error Detected", "RW1C", bit.at(2));
		if (bit.at(2) == '0')
			PrintRegisterDescription("Fatal errors NOT detected");
		if (bit.at(2) == '1')
			PrintRegisterDescription("Fatal errors detected");
		PrintRegisterValue(FALSE, "Unsupported Request Detected", "RW1C", bit.at(3));
		if (bit.at(3) == '0')
			PrintRegisterDescription("Unsupported Requests NOT detected");
		if (bit.at(3) == '1')
			PrintRegisterDescription("Unsupported Request detected");
		PrintRegisterValue(FALSE, "AUX Power Detected", "RO", bit.at(4));
		if (bit.at(4) == '0')
			PrintRegisterDescription("Aux power is NOT detected");
		if (bit.at(4) == '1')
			PrintRegisterDescription("Aux power is detected");
		PrintRegisterValue(FALSE, "Transactions Pending", "RO", bit.at(5));
		if (bit.at(5) == '0')
			PrintRegisterDescription("All outstanding Non-Posted Requests have completed or have been terminated by the Completion Timeout mechanism");
		if (bit.at(5) == '1')
			PrintRegisterDescription("The Function has issued Non-Posted Requests that have not been completed");
		if (bit.at(6) == '0') {
			PrintRegisterValue(TRUE, "Undefined", "RO", ReverseString(bit.substr(6, 10)));
		}
		if (bit.at(6) == '1') {
			PrintRegisterValue(TRUE, "Emergency Power Reduction Detected", "RW1C", bit.at(6));
			PrintRegisterValue(TRUE, "Undefined", "RO", ReverseString(bit.substr(7, 9)));
		}
		ValueHighlight(FALSE, "Link Capabilities", pci_express_capability.link_capabilities, 8);
		bit = ReverseString(bitset<32>(pci_express_capability.link_capabilities).to_string());
		PrintRegisterValue(FALSE, "Max Link Speed", "RO", ReverseString(bit.substr(0, 4)));
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x01)
			PrintRegisterDescription("2.5 GT/sec (Supported Link Speeds Vector field bit 0)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x02)
			PrintRegisterDescription("5.0 GT/sec (Supported Link Speeds Vector field bit 1)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x03)
			PrintRegisterDescription("8.0 GT/sec (Supported Link Speeds Vector field bit 2)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x04)
			PrintRegisterDescription("16.0 GT/sec (Supported Link Speeds Vector field bit 3)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x05)
			PrintRegisterDescription("32.0 GT/sec (Supported Link Speeds Vector field bit 4)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x06)
			PrintRegisterDescription("Reserved (Supported Link Speeds Vectorfield bit 5)");
		if (stoi(ReverseString(bit.substr(0, 4)), 0, 2) == 0x07)
			PrintRegisterDescription("Reserved (Supported Link Speeds Vectorfield bit 6)");
		PrintRegisterValue(FALSE, "Maximum Link Width", "RO", ReverseString(bit.substr(4, 6)));
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x00)
			PrintRegisterDescription("Reserved");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x01)
			PrintRegisterDescription("x1");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x02)
			PrintRegisterDescription("x2");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x04)
			PrintRegisterDescription("x4");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x08)
			PrintRegisterDescription("x8");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x0C)
			PrintRegisterDescription("x12");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x10)
			PrintRegisterDescription("x16");
		if (stoi(ReverseString(bit.substr(4, 6)), 0, 2) == 0x20)
			PrintRegisterDescription("x32");
		PrintRegisterValue(FALSE, "Active State Power Management Support", "RO", ReverseString(bit.substr(10, 2)));
		if (stoi(ReverseString(bit.substr(10, 2)), 0, 2) == 0x00)
			PrintRegisterDescription("No ASPM Support");
		if (stoi(ReverseString(bit.substr(10, 2)), 0, 2) == 0x01)
			PrintRegisterDescription("L0s Supported");
		if (stoi(ReverseString(bit.substr(10, 2)), 0, 2) == 0x02)
			PrintRegisterDescription("L1 Supported");
		if (stoi(ReverseString(bit.substr(10, 2)), 0, 2) == 0x04)
			PrintRegisterDescription("L0s and L1 Supported");
		PrintRegisterValue(FALSE, "L0s Exit Latency", "RO", ReverseString(bit.substr(12, 3)));
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("Less than 64 ns");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("64 ns to less than 128 ns");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("128 ns to less than 256 ns");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("256 ns to less than 512 ns");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("512 ns to less than 1 レs");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("1 レs to less than 2 レs");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("2 レs - 4 レs");
		if (stoi(ReverseString(bit.substr(12, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("More than 4 レs");
		PrintRegisterValue(FALSE, "L0s Exit Latency", "RO", ReverseString(bit.substr(15, 3)));
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x00)
			PrintRegisterDescription("Less than 1 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x01)
			PrintRegisterDescription("1 レs to less than 2 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x02)
			PrintRegisterDescription("2 レs to less than 4 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x03)
			PrintRegisterDescription("4 レs to less than 8 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x04)
			PrintRegisterDescription("8 レs to less than 16 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x05)
			PrintRegisterDescription("16 レs to less than 32 レs");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x06)
			PrintRegisterDescription("32 レs - 64 レs ");
		if (stoi(ReverseString(bit.substr(15, 3)), 0, 2) == 0x07)
			PrintRegisterDescription("More than 64 レs");
		PrintRegisterValue(TRUE, "Clock Power Management", "RO", bit.at(18));
		PrintRegisterValue(TRUE, "Surprise Down Error Reporting Capable", "RO", bit.at(19));
		PrintRegisterValue(TRUE, "Data Link Layer Link Active Reporting Capable", "RO", bit.at(20));
		PrintRegisterValue(TRUE, "Link Bandwidth Notification Capability", "RO", bit.at(21));
		PrintRegisterValue(TRUE, "ASPM Optionality Compliance", "HWINIT", bit.at(22));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", bit.at(23));
		PrintRegisterValue(TRUE, "Port Number", "HWINIT", ReverseString(bit.substr(24, 8)));
		ValueHighlight(FALSE, "Link Control", pci_express_capability.link_control, 4);
		bit = ReverseString(bitset<16>(pci_express_capability.link_control).to_string());
		PrintRegisterValue(FALSE, "Active State Power Management Control", "RW", ReverseString(bit.substr(0, 2)));
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x00)
			PrintRegisterDescription("Disabled ");
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x01)
			PrintRegisterDescription("L0s Entry Enabled");
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x02)
			PrintRegisterDescription("L1 Entry Enabled");
		if (stoi(ReverseString(bit.substr(0, 2)), 0, 2) == 0x03)
			PrintRegisterDescription("L0s and L1 Entry Enabled");
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", bit.at(2));
		PrintRegisterValue(FALSE, "Transactions Pending", "RO", bit.at(3));
		if (bit.at(3) == '0')
			PrintRegisterDescription("64 byte");
		if (bit.at(3) == '1')
			PrintRegisterDescription("128 byte");
		ValueHighlight(FALSE, "Link Status", pci_express_capability.link_status, 4);
		ValueHighlight(FALSE, "Slot Capabilities", pci_express_capability.slot_capabilities, 8);
		ValueHighlight(FALSE, "Slot Control", pci_express_capability.slot_control, 4);
		ValueHighlight(FALSE, "Slot Status", pci_express_capability.slot_status, 4);
		ValueHighlight(FALSE, "Root Control", pci_express_capability.root_control, 4);
		ValueHighlight(FALSE, "Root Capabilities", pci_express_capability.root_capabilities, 4);
		ValueHighlight(FALSE, "Root Status", pci_express_capability.root_status, 8);
		ValueHighlight(FALSE, "Device Capabilities 2", pci_express_capability.device_capabilities_2, 8);
		ValueHighlight(FALSE, "Device Control 2", pci_express_capability.device_control_2, 4);
		ValueHighlight(FALSE, "Device Status 2", pci_express_capability.device_status_2, 4);
		ValueHighlight(FALSE, "Link Capabilities 2", pci_express_capability.link_capabilities_2, 8);
		ValueHighlight(FALSE, "Link Control 2", pci_express_capability.link_control_2, 4);
		ValueHighlight(FALSE, "Link Status 2", pci_express_capability.link_status_2, 4);
		ValueHighlight(FALSE, "Slot Capabilities 2", pci_express_capability.slot_capabilities_2, 8);
		ValueHighlight(FALSE, "Slot Control 2", pci_express_capability.slot_control_2, 4);
		ValueHighlight(FALSE, "Slot Status 2", pci_express_capability.slot_status_2, 4);
	}
	*/
}