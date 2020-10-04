#include <Windows.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <string>

#include "pci_power_management_interface.h"
#include "../capability.h"
#include "../pci.h"

using namespace std;

PciPowerManagementInterfaceCapability pci_power_management_interface_capability = { 0, };

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

void PrintPciPowerManagementInterfaceCapability(void) {
	if (pci_power_management_interface_capability.exists) {
		CapabilityHighlight("PCI Power Management Capability", pci_power_management_interface_capability.offset);
		ValueHighlight(FALSE, "Capability ID", pci_power_management_interface_capability.capability_id, 2);
		ValueHighlight(FALSE, "Next Capability Pointer", pci_power_management_interface_capability.next_capability_pointer, 2);
		ValueHighlight(TRUE, "Power Management Capabilities", pci_power_management_interface_capability.power_management_capabilities, 4);
		PrintRegisterValue(FALSE, "Version", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x7, 0));
		switch (GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x7, 0)) {
		case 0x01:
			PrintRegisterDescription("PCI Power Management Interface Specification rev.1.0");
			break;
		case 0x02:
			PrintRegisterDescription("PCI Power Management Interface Specification rev.1.1");
			break;
		case 0x03:
			PrintRegisterDescription("PCI Power Management Interface Specification rev.1.2");
			break;
		}
		PrintRegisterValue(FALSE, "PME Clock", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x8, 3));
		switch (GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x8, 3)) {
		case 0x00:
			PrintRegisterDescription("Indicates that no PCI clock is required for the function to generate PME#");
			break;
		case 0x01:
			PrintRegisterDescription("Indicates that the function relies on the presence of the PCI clock for PME# operation");
			break;
		}
		PrintRegisterValue(TRUE, "Immediate_Readiness_on_Return_to_D0", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x10, 4));
		PrintRegisterValue(TRUE, "Device Specific Initialization", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x20, 5));
		PrintRegisterValue(FALSE, "Aux_Current", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x1C0, 6));
		switch (GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x1C0, 6)) {
		case 0x00:
			PrintRegisterDescription("0mA (self powered)");
			break;
		case 0x01:
			PrintRegisterDescription("55mA");
			break;
		case 0x02:
			PrintRegisterDescription("100mA");
			break;
		case 0x03:
			PrintRegisterDescription("160mA");
			break;
		case 0x04:
			PrintRegisterDescription("220mA");
			break;
		case 0x05:
			PrintRegisterDescription("270mA");
			break;
		case 0x06:
			PrintRegisterDescription("320mA");
			break;
		case 0x07:
			PrintRegisterDescription("375mA");
			break;
		}
		PrintRegisterValue(TRUE, "D1_Support", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x200, 9));
		PrintRegisterValue(TRUE, "D2_Support", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0x400, 10));
		PrintRegisterValue(TRUE, "PME_Support", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_capabilities, 0xF800, 11));
		ValueHighlight(TRUE, "Power Management Control/Status", pci_power_management_interface_capability.power_management_control_status, 4);
		PrintRegisterValue(FALSE, "PowerState", "RW", GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x3, 0));
		switch (GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x3, 0)) {
		case 0x00:
			PrintRegisterDescription("D0");
			break;
		case 0x01:
			PrintRegisterDescription("D1");
			break;
		case 0x02:
			PrintRegisterDescription("D2");
			break;
		case 0x03:
			PrintRegisterDescription("D3_Hot");
			break;
		}
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x4, 2));
		PrintRegisterValue(FALSE, "No_Soft_Reset", "RO", GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x8, 3));
		switch (GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x8, 3)) {
		case 0x00:
			PrintRegisterDescription("Devices do perform an internal reset upon transitioning from D3hot to D0 via software control of the PowerState bits.");
			break;
		case 0x01:
			PrintRegisterDescription("Devices do NOT perform an internal reset upon transitioning from D3hot to D0 via software control of the PowerState bits.");
			break;
		}
		PrintRegisterValue(TRUE, "Reserved", "RSVDP", GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0xF0, 4));
		PrintRegisterValue(TRUE, "PME_En", "RW", GetRegisterValue(pci_power_management_interface_capability.power_management_control_status, 0x100, 8));
		switch (pci_power_management_interface_capability.power_management_control_status, 0x100, 8) {
		case 0x00:
			PrintRegisterDescription("Assertion is disabled");
			break;
		case 0x01:
			PrintRegisterDescription("Assertion is enabled");
			break;
		}
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