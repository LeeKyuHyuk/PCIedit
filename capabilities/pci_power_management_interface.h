#pragma once

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

void GetPciPowerManagementInterfaceCapability(int bus, int device, int function, BYTE offset);
void PrintPciPowerManagementInterfaceCapability(void);