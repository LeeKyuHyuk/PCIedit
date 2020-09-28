#pragma once

#define MAX_PCI_DEVICE_NUMBER 55336

typedef struct PciDevice {
	unsigned short index;
	unsigned short bus;
	unsigned short device;
	unsigned short function;
	WORD vendor_id;
	WORD device_id;
	DWORD class_code;
} PciDevice;

void Title(void);
void Help(void);
void FindPciDevice(void);
void GetPciDevice(HANDLE console);
void SelectPciMessage(HANDLE console, PciDevice pci_device);
void ErrorMessage(HANDLE console, std::string message);
BOOL ExtractResource(uint16_t resource_id, LPCWSTR output_filename, LPCWSTR resource_type);
BOOL WINAPI ConsoleHandler(DWORD CEvent);