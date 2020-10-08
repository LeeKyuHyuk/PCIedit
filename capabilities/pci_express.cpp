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
	/*
	ReadPciDword(bus, device, function, offset + 0x34, &data);
	pci_express_capability.slot_capabilities_2 = data[1];
	ReadPciDword(bus, device, function, offset + 0x38, &data);
	pci_express_capability.slot_control_2 = LOWORD(data[1]);
	pci_express_capability.slot_status_2 = HIWORD(data[1]);
	*/
	pci_express_capability.exists = TRUE;
}

void PrintPciExpressCapability(void) {
	if (pci_express_capability.exists) {
		CapabilityHighlight("PCI Express Capability", pci_express_capability.offset);
		ValueHighlight(FALSE, "Capability ID", pci_express_capability.capability_id, 2);
		ValueHighlight(FALSE, "Next Capability Pointer", pci_express_capability.next_capability_pointer, 2);
		ValueHighlight(TRUE, "PCI Express Capabilities", pci_express_capability.pci_express_capabilities, 4);
		PrintRegisterValue(FALSE, "Capability Version", "RO", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0xF, 0));
		switch (GetRegisterValue(pci_express_capability.pci_express_capabilities, 0xF, 0)) {
		case 0x1:
			PrintRegisterDescription("Version 1 (Compliant with PCI Express Base Specification rev 1.0)");
			break;
		case 0x2:
			PrintRegisterDescription("Version 2 (Compliant with PCI Express Base Specification rev 2.0)");
			break;
		}
		PrintRegisterValue(FALSE, "Device/Port Type", "RO", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0xF, 4));
		if (type_0_configuration_space_header.exists) {
			switch (GetRegisterValue(pci_express_capability.pci_express_capabilities, 0xF, 4)) {
			case 0x00:
				PrintRegisterDescription("PCI Express Endpoint");
				break;
			case 0x01:
				PrintRegisterDescription("Legacy PCI Express Endpoint");
				break;
			case 0x09:
				PrintRegisterDescription("Root Complex Integrated Endpoint");
				break;
			case 0x0A:
				PrintRegisterDescription("Root Complex Event Collector");
			}	
		}
		if (type_1_configuration_space_header.exists) {
			switch (GetRegisterValue(pci_express_capability.pci_express_capabilities, 0xF, 4)) {
			case 0x04:
				PrintRegisterDescription("Root Port of PCI Express Root Complex");
				break;
			case 0x05:
				PrintRegisterDescription("Upstream Port of PCI Express Switch");
				break;
			case 0x06:
				PrintRegisterDescription("Downstream Port of PCI Express Switch");
				break;
			case 0x07:
				PrintRegisterDescription("PCI Express to PCI/PCI-X Bridge");
				break;
			case 0x08:
				PrintRegisterDescription("PCI/PCI-X to PCI Express Bridge");
				break;
			}	
		}
		PrintRegisterValue(FALSE, "Slot Implemented", "HWINIT", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0x1, 8));
		switch (GetRegisterValue(pci_express_capability.pci_express_capabilities, 0x1, 8)) {
		case 0x00:
			PrintRegisterDescription("Link is NOT connected to a slot");
			break;
		case 0x01:
			PrintRegisterDescription("Link is connected to a slot");
			break;
		}	
		PrintRegisterValue(TRUE, "Interrupt Message Number", "RO", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0x1F, 9));
		PrintRegisterValue(TRUE, "Undefined", "RO", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0x1, 14));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_express_capability.pci_express_capabilities, 0x1, 15));
		ValueHighlight(TRUE, "Device Capabilities", pci_express_capability.device_capabilities, 8);
		PrintRegisterValue(FALSE, "Max_Payload_Size Supported", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 0));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 0)) {
		case 0x00:
			PrintRegisterDescription("128 bytes max payload size");
			break;
		case 0x01:
			PrintRegisterDescription("256 bytes max payload size");
			break;
		case 0x02:
			PrintRegisterDescription("512 bytes max payload size");
			break;
		case 0x03:
			PrintRegisterDescription("1024 bytes max payload size");
			break;
		case 0x04:
			PrintRegisterDescription("2048 bytes max payload size");
			break;
		case 0x05:
			PrintRegisterDescription("4096 bytes max payload size");
			break;
		case 0x06:
			PrintRegisterDescription("Reserved");
			break;
		case 0x07:
			PrintRegisterDescription("Reserved");
			break;
		}
		PrintRegisterValue(FALSE, "Phantom Functions Supported", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x3, 3));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x3, 3)) {
		case 0x00:
			PrintRegisterDescription("No Function Number bits are used for Phantom Functions. Multi-Function Devices are permitted to implement up to 8 independent Functions.");
			break;
		case 0x01:
			PrintRegisterDescription("The most significant bit of the Function number in Requester ID is used for Phantom Functions; a Multi-Function Device is permitted to implement Functions 0-3. Functions 0, 1, 2, and 3 are permitted to use Function Numbers 4, 5, 6, and 7 respectively as Phantom Functions.");
			break;
		case 0x02:
			PrintRegisterDescription("The two most significant bits of Function Number in Requester ID are used for Phantom Functions; a Multi-Function Device is permitted to implement Functions 0-1. Function 0 is permitted to use Function Numbers 2, 4, and 6 for Phantom Functions. Function 1 is permitted to use Function Numbers 3, 5, and 7 as Phantom Functions. ");
			break;
		case 0x03:
			PrintRegisterDescription("All 3 bits of Function Number in Requester ID used for Phantom Functions. The device must have a single Function 0 that is permitted to use all other Function Numbers as Phantom Functions.");
			break;
		}	
		PrintRegisterValue(FALSE, "Extended Tag Field Supported", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 5));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 5)) {
		case 0x00:
			PrintRegisterDescription("5-bit Tag field supported");
			break;
		case 0x01:
			PrintRegisterDescription("8-bit Tag field supported");
			break;
		}	
		PrintRegisterValue(FALSE, "Endpoint L0s Acceptable Latency", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 6));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 6)) {
		case 0x00:
			PrintRegisterDescription("Maximum of 64 ns");
			break;
		case 0x01:
			PrintRegisterDescription("Maximum of 128 ns");
			break;
		case 0x02:
			PrintRegisterDescription("Maximum of 256 ns");
			break;
		case 0x03:
			PrintRegisterDescription("Maximum of 512 ns");
			break;
		case 0x04:
			PrintRegisterDescription("Maximum of 1 レs");
			break;
		case 0x05:
			PrintRegisterDescription("Maximum of 2 レs");
			break;
		case 0x06:
			PrintRegisterDescription("Maximum of 4 レs");
			break;
		case 0x07:
			PrintRegisterDescription("No limit");
			break;
		}
		PrintRegisterValue(FALSE, "Endpoint L1 Acceptable Latency", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 9));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 9)) {
		case 0x00:
			PrintRegisterDescription("Maximum of 1 レs");
			break;
		case 0x01:
			PrintRegisterDescription("Maximum of 2 レs");
			break;
		case 0x02:
			PrintRegisterDescription("Maximum of 4 レs");
			break;
		case 0x03:
			PrintRegisterDescription("Maximum of 8 レs");
			break;
		case 0x04:
			PrintRegisterDescription("Maximum of 16 レs");
			break;
		case 0x05:
			PrintRegisterDescription("Maximum of 32 レs");
			break;
		case 0x06:
			PrintRegisterDescription("Maximum of 64 レs");
			break;
		case 0x07:
			PrintRegisterDescription("No limit");
			break;
		}
		PrintRegisterValue(TRUE, "Undefined", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 12));
		PrintRegisterValue(FALSE, "Role-Based Error Reporting", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 15));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 15)) {
		case 0x00:
			PrintRegisterDescription("Function does NOT implement the functionality defined in the Error Reporting ECN");
			break;
		case 0x01:
			PrintRegisterDescription("Function implements the functionality defined in the Error Reporting ECN");
			break;
		}
		PrintRegisterValue(TRUE, "ERR_COR Subclass Capable", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 16));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 17));
		PrintRegisterValue(TRUE, "Captured Slot Power Limit Value", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0xFF, 18));
		PrintRegisterValue(FALSE, "Captured Slot Power Limit Scale", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x3, 26));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x3, 26)) {
		case 0x00:
			PrintRegisterDescription("1.0x");
			break;
		case 0x01:
			PrintRegisterDescription("0.1x");
			break;
		case 0x02:
			PrintRegisterDescription("0.01x");
			break;
		case 0x03:
			PrintRegisterDescription("0.001x");
			break;
		}
		PrintRegisterValue(FALSE, "Function Level Reset Capability", "RO", GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 28));
		switch (GetRegisterValue(pci_express_capability.device_capabilities, 0x1, 28)) {
		case 0x00:
			PrintRegisterDescription("Function does NOT support the Function Level Reset mechanism");
			break;
		case 0x01:
			PrintRegisterDescription("Function supports the Function Level Reset mechanism");
			break;
		}	
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_express_capability.device_capabilities, 0x7, 29));
		ValueHighlight(TRUE, "Device Control", pci_express_capability.device_control, 4);
		PrintRegisterValue(FALSE, "Correctable Error Reporting Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 0));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Sending ERR_COR Message is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Sending ERR_COR Message is enabled");
			break;
		}	
		PrintRegisterValue(FALSE, "Non-Fatal Error Reporting Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 1));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Sending ERR_NONFATAL Message is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Sending ERR_NONFATAL Message is enabled");
			break;
		}
		PrintRegisterValue(FALSE, "Fatal Error Reporting Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 2));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 2)) {
		case 0x00:
			PrintRegisterDescription("Sending ERR_FATAL Message is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Sending ERR_FATAL Message is enabled");
			break;
		}	
		PrintRegisterValue(FALSE, "Unsupported Request Reporting Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 3));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 3)) {
		case 0x00:
			PrintRegisterDescription("Signaling of Unsupported Request Errors is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Signaling of Unsupported Request Errors is enabled");
			break;
		}	
		PrintRegisterValue(FALSE, "Enable Relaxed Ordering", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 4));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 4)) {
		case 0x00:
			PrintRegisterDescription("Function is NOT permitted to set the Relaxed Ordering bit");
			break;
		case 0x01:
			PrintRegisterDescription("Function is permitted to set the Relaxed Ordering bit");
			break;
		}	
		PrintRegisterValue(FALSE, "Max_Payload_Size", "RW", GetRegisterValue(pci_express_capability.device_control, 0x7, 5));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x7, 5)) {
		case 0x00:
			PrintRegisterDescription("128 bytes max payload size");
			break;
		case 0x01:
			PrintRegisterDescription("256 bytes max payload size");
			break;
		case 0x02:
			PrintRegisterDescription("512 bytes max payload size");
			break;
		case 0x03:
			PrintRegisterDescription("1024 bytes max payload size");
			break;
		case 0x04:
			PrintRegisterDescription("2048 bytes max payload size");
			break;
		case 0x05:
			PrintRegisterDescription("4096 bytes max payload size");
			break;
		case 0x06:
			PrintRegisterDescription("Reserved");
			break;
		case 0x07:
			PrintRegisterDescription("Reserved");
			break;
		}
		PrintRegisterValue(FALSE, "Extended Tag Field Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 8));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 8)) {
		case 0x00:
			PrintRegisterDescription("The Function is restricted to a 5-bit Tag field");
			PrintRegisterDescription("Enables the Function to use an 8-bit Tag field");
			break;
		case 0x01:
			break;
		}
		PrintRegisterValue(FALSE, "Phantom Functions Enable", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 9));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 9)) {
		case 0x00:
			PrintRegisterDescription("The Function is NOT allowed to use Phantom Functions");
			break;
		case 0x01:
			PrintRegisterDescription("The Function is allowed to use Phantom Functions");
			break;
		}
		PrintRegisterValue(FALSE, "Aux Power PM Enable", "RWS", GetRegisterValue(pci_express_capability.device_control, 0x1, 10));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 10)) {
		case 0x00:
			PrintRegisterDescription("The Function is NOT enabled allowed to draw Aux power");
			break;
		case 0x01:
			PrintRegisterDescription("The Function is enabled allowed to draw Aux power");
			break;
		}
		PrintRegisterValue(FALSE, "Enable No Snoop", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 11));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x1, 10)) {
		case 0x00:
			PrintRegisterDescription("The Function is NOT permitted to set the No Snoop bit");
			break;
		case 0x01:
			PrintRegisterDescription("The Function is permitted to set the No Snoop bit");
			break;
		}
		PrintRegisterValue(FALSE, "Max_Read_Request_Size", "RW", GetRegisterValue(pci_express_capability.device_control, 0x7, 12));
		switch (GetRegisterValue(pci_express_capability.device_control, 0x7, 12)) {
		case 0x00:
			PrintRegisterDescription("128 bytes max payload size");
			break;
		case 0x01:
			PrintRegisterDescription("256 bytes max payload size");
			break;
		case 0x02:
			PrintRegisterDescription("512 bytes max payload size");
			break;
		case 0x03:
			PrintRegisterDescription("1024 bytes max payload size");
			break;
		case 0x04:
			PrintRegisterDescription("2048 bytes max payload size");
			break;
		case 0x05:
			PrintRegisterDescription("4096 bytes max payload size");
			break;
		case 0x06:
			PrintRegisterDescription("Reserved");
			break;
		case 0x07:
			PrintRegisterDescription("Reserved");
			break;
		}
		PrintRegisterValue(TRUE, "Initiate Function Level Reset", "RW", GetRegisterValue(pci_express_capability.device_control, 0x1, 15));
		ValueHighlight(TRUE, "Device Status", pci_express_capability.device_status, 4);
		PrintRegisterValue(FALSE, "Correctable Error Detected", "RW1C", GetRegisterValue(pci_express_capability.device_status, 0x1, 0));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 0)) {
		case 0x00:
			PrintRegisterDescription("Correctable errors NOT detected");
			break;
		case 0x01:
			PrintRegisterDescription("Correctable errors detected");
			break;
		}
		PrintRegisterValue(FALSE, "Non-Fatal Error Detected", "RW1C", GetRegisterValue(pci_express_capability.device_status, 0x1, 1));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 1)) {
		case 0x00:
			PrintRegisterDescription("Non-fatal errors NOT detected");
			break;
		case 0x01:
			PrintRegisterDescription("Non-fatal errors detected");
			break;
		}
		PrintRegisterValue(FALSE, "Fatal Error Detected", "RW1C", GetRegisterValue(pci_express_capability.device_status, 0x1, 2));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 2)) {
		case 0x00:
			PrintRegisterDescription("Fatal errors NOT detected");
			break;
		case 0x01:
			PrintRegisterDescription("Fatal errors detected");
			break;
		}
		PrintRegisterValue(FALSE, "Unsupported Request Detected", "RW1C", GetRegisterValue(pci_express_capability.device_status, 0x1, 3));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 3)) {
		case 0x00:
			PrintRegisterDescription("Unsupported Requests NOT detected");
			break;
		case 0x01:
			PrintRegisterDescription("Unsupported Request detected");
			break;
		}
		PrintRegisterValue(FALSE, "AUX Power Detected", "RO", GetRegisterValue(pci_express_capability.device_status, 0x1, 4));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 4)) {
		case 0x00:
			PrintRegisterDescription("Aux power is NOT detected");
			break;
		case 0x01:
			PrintRegisterDescription("Aux power is detected");
			break;
		}
		PrintRegisterValue(FALSE, "Transactions Pending", "RO", GetRegisterValue(pci_express_capability.device_status, 0x1, 5));
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 5)) {
		case 0x00:
			PrintRegisterDescription("All outstanding Non-Posted Requests have completed or have been terminated by the Completion Timeout mechanism");
			break;
		case 0x01:
			PrintRegisterDescription("The Function has issued Non-Posted Requests that have not been completed");
			break;
		}
		switch (GetRegisterValue(pci_express_capability.device_status, 0x1, 6)) {
		case 0x00:
			PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(pci_express_capability.device_status, 0x3FF, 6));
			break;
		case 0x01:
			PrintRegisterValue(TRUE, "Emergency Power Reduction Detected", "RW1C", GetRegisterValue(pci_express_capability.device_status, 0x1, 6));
			PrintRegisterValue(TRUE, "Reserved", "RSVDZ", GetRegisterValue(pci_express_capability.device_status, 0x1FF, 7));
			break;
		}
		ValueHighlight(TRUE, "Link Capabilities", pci_express_capability.link_capabilities, 8);
		PrintRegisterValue(FALSE, "Max Link Speed", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0xF, 0));
		switch (GetRegisterValue(pci_express_capability.link_capabilities, 0xF, 0)) {
		case 0x01:
			PrintRegisterDescription("2.5 GT/sec (Supported Link Speeds Vector field bit 0)");
			break;
		case 0x02:
			PrintRegisterDescription("5.0 GT/sec (Supported Link Speeds Vector field bit 1)");
			break;
		case 0x03:
			PrintRegisterDescription("8.0 GT/sec (Supported Link Speeds Vector field bit 2)");
			break;
		case 0x04:
			PrintRegisterDescription("16.0 GT/sec (Supported Link Speeds Vector field bit 3)");
			break;
		case 0x05:
			PrintRegisterDescription("32.0 GT/sec (Supported Link Speeds Vector field bit 4)");
			break;
		case 0x06:
			PrintRegisterDescription("Reserved (Supported Link Speeds Vectorfield bit 5)");
			break;
		case 0x07:
			PrintRegisterDescription("Reserved (Supported Link Speeds Vectorfield bit 6)");
			break;
		}
		PrintRegisterValue(FALSE, "Maximum Link Width", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x3F, 4));
		switch (GetRegisterValue(pci_express_capability.link_capabilities, 0x3F, 4)) {
		case 0x00:
			PrintRegisterDescription("Reserved");
			break;
		case 0x01:
			PrintRegisterDescription("x1");
			break;
		case 0x02:
			PrintRegisterDescription("x2");
			break;
		case 0x04:
			PrintRegisterDescription("x4");
			break;
		case 0x08:
			PrintRegisterDescription("x8");
			break;
		case 0x0C:
			PrintRegisterDescription("x12");
			break;
		case 0x10:
			PrintRegisterDescription("x16");
			break;
		case 0x20:
			PrintRegisterDescription("x32");
			break;
		}
		PrintRegisterValue(FALSE, "Active State Power Management Support", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x3, 10));
		switch (GetRegisterValue(pci_express_capability.link_capabilities, 0x3, 10)) {
		case 0x00:
			PrintRegisterDescription("No ASPM Support");
			break;
		case 0x01:
			PrintRegisterDescription("L0s Supported");
			break;
		case 0x02:
			PrintRegisterDescription("L1 Supported");
			break;
		case 0x03:
			PrintRegisterDescription("L0s and L1 Supported");
			break;
		}
		PrintRegisterValue(FALSE, "L0s Exit Latency", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x7, 12));
		switch (GetRegisterValue(pci_express_capability.link_capabilities, 0x7, 12)) {
		case 0x00:
			PrintRegisterDescription("Less than 64 ns");
			break;
		case 0x01:
			PrintRegisterDescription("64 ns to less than 128 ns");
			break;
		case 0x02:
			PrintRegisterDescription("128 ns to less than 256 ns");
			break;
		case 0x03:
			PrintRegisterDescription("256 ns to less than 512 ns");
			break;
		case 0x04:
			PrintRegisterDescription("512 ns to less than 1 レs");
			break;
		case 0x05:
			PrintRegisterDescription("1 レs to less than 2 レs");
			break;
		case 0x06:
			PrintRegisterDescription("2 レs - 4 レs");
			break;
		case 0x07:
			PrintRegisterDescription("More than 4 レs");
			break;
		}
		PrintRegisterValue(FALSE, "L1 Exit Latency", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x7, 15));
		switch (GetRegisterValue(pci_express_capability.link_capabilities, 0x7, 15)) {
		case 0x00:
			PrintRegisterDescription("Less than 1 レs");
			break;
		case 0x01:
			PrintRegisterDescription("1 レs to less than 2 レs");
			break;
		case 0x02:
			PrintRegisterDescription("2 レs to less than 4 レs");
			break;
		case 0x03:
			PrintRegisterDescription("4 レs to less than 8 レs");
			break;
		case 0x04:
			PrintRegisterDescription("8 レs to less than 16 レs");
			break;
		case 0x05:
			PrintRegisterDescription("16 レs to less than 32 レs");
			break;
		case 0x06:
			PrintRegisterDescription("32 レs - 64 レs ");
			break;
		case 0x07:
			PrintRegisterDescription("More than 64 レs");
			break;
		}
		PrintRegisterValue(TRUE, "Clock Power Management", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 18));
		PrintRegisterValue(TRUE, "Surprise Down Error Reporting Capable", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 19));
		PrintRegisterValue(TRUE, "Data Link Layer Link Active Reporting Capable", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 20));
		PrintRegisterValue(TRUE, "Link Bandwidth Notification Capability", "RO", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 21));
		PrintRegisterValue(TRUE, "ASPM Optionality Compliance", "HWINIT", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 22));
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_express_capability.link_capabilities, 0x1, 23));
		PrintRegisterValue(TRUE, "Port Number", "HWINIT", GetRegisterValue(pci_express_capability.link_capabilities, 0xFF, 24));
		ValueHighlight(TRUE, "Link Control", pci_express_capability.link_control, 4);
		PrintRegisterValue(FALSE, "Active State Power Management Control", "RW", GetRegisterValue(pci_express_capability.link_control, 0x3, 0));
		switch (GetRegisterValue(pci_express_capability.link_control, 0x3, 0)) {
		case 0x00:
			PrintRegisterDescription("Disabled");
			break;
		case 0x01:
			PrintRegisterDescription("L0s Entry Enabled");
			break;
		case 0x02:
			PrintRegisterDescription("L1 Entry Enabled");
			break;
		case 0x03:
			PrintRegisterDescription("L0s and L1 Entry Enabled");
			break;
		}
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_express_capability.link_control, 0x1, 2));
		PrintRegisterValue(FALSE, "Transactions Pending", "RO", GetRegisterValue(pci_express_capability.link_control, 0x1, 3));
		switch (GetRegisterValue(pci_express_capability.link_control, 0x1, 3)) {
		case 0x00:
			PrintRegisterDescription("64 byte");
			break;
		case 0x01:
			PrintRegisterDescription("128 byte");
			break;
		}
		ValueHighlight(TRUE, "Link Status", pci_express_capability.link_status, 4);
		ValueHighlight(FALSE, "Slot Capabilities", pci_express_capability.slot_capabilities, 8);
		ValueHighlight(FALSE, "Slot Control", pci_express_capability.slot_control, 4);
		ValueHighlight(FALSE, "Slot Status", pci_express_capability.slot_status, 4);
		ValueHighlight(FALSE, "Root Control", pci_express_capability.root_control, 4);
		ValueHighlight(FALSE, "Root Capabilities", pci_express_capability.root_capabilities, 4);
		ValueHighlight(FALSE, "Root Status", pci_express_capability.root_status, 8);
		ValueHighlight(TRUE, "Device Capabilities 2", pci_express_capability.device_capabilities_2, 8);
		ValueHighlight(TRUE, "Device Control 2", pci_express_capability.device_control_2, 4);
		ValueHighlight(FALSE, "Device Status 2", pci_express_capability.device_status_2, 4);
		ValueHighlight(TRUE, "Link Capabilities 2", pci_express_capability.link_capabilities_2, 8);
		ValueHighlight(TRUE, "Link Control 2", pci_express_capability.link_control_2, 4);
		ValueHighlight(TRUE, "Link Status 2", pci_express_capability.link_status_2, 4);
		/*
		ValueHighlight(FALSE, "Slot Capabilities 2", pci_express_capability.slot_capabilities_2, 8);
		ValueHighlight(FALSE, "Slot Control 2", pci_express_capability.slot_control_2, 4);
		ValueHighlight(FALSE, "Slot Status 2", pci_express_capability.slot_status_2, 4);
		*/
	}
}