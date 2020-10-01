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