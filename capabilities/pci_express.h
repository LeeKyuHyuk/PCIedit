#pragma once

typedef struct PciExpressCapability {
	BOOL exists;
	BYTE offset;
	BYTE capability_id;
	BYTE next_capability_pointer;
	WORD pci_express_capabilities;
	DWORD device_capabilities;
	WORD device_control;
	WORD device_status;
	DWORD link_capabilities;
	WORD link_control;
	WORD link_status;
	DWORD slot_capabilities;
	WORD slot_control;
	WORD slot_status;
	WORD root_control;
	WORD root_capabilities;
	DWORD root_status;
	DWORD device_capabilities_2;
	WORD device_control_2;
	WORD device_status_2;
	DWORD link_capabilities_2;
	WORD link_control_2;
	WORD link_status_2;
	DWORD slot_capabilities_2;
	WORD slot_control_2;
	WORD slot_status_2;
} PciExpressCapability;

void GetPciExpressCapability(int bus, int device, int function, BYTE offset);
void PrintPciExpressCapability(void);