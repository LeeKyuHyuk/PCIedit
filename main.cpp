#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "main.h"
#include "capability.h"
#include "pci.h"
#include "resource.h"

using namespace std;

vector<PciDevice> pci_device_vector;

int main() {
	int select_pcie_device = -1;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	string command = "";

	/* Extract RwDrv Driver */
	ExtractResource(IDR_BINARY1, L"RwDrv.sys", L"BINARY");

	/* Load Driver */
	BOOL load_driver_status = LoadPhyMemDriver();
	if (load_driver_status == FALSE) {
		cout << "Load 'RwDrv.sys' driver failed!" << endl;
		system("PAUSE");
		return EXIT_FAILURE;
	}

	/* Select PCI Device (Input Bus, Device, Function) */
	Title();
	FindPciDevice();
	GetPciDevice();
	do {
		cout << ">> Select PCI Device : ";
		cin >> select_pcie_device;
		if (cin.fail() || pci_device_vector.size() < (select_pcie_device + 1)) {
			cin.clear();
			cin.ignore();
			ErrorMessage(console, "Please select a valid PCI device!");
			select_pcie_device = -1;
		}
	} while (select_pcie_device < 0);
	SelectPciMessage(console, pci_device_vector[select_pcie_device]);

	/* PCIedit Colsone */
	while (TRUE) {
		//WritePCIDW(bus, dev, func, 0, 0x12345678);
		cout << "[PCIedit] >> ";
		cin >> command;
		cout << endl;
		try {
			if (strcmp(command.c_str(), "h") == 0 || strcmp(command.c_str(), "H") == 0)
				Help();
			else if (strcmp(command.c_str(), "r") == 0 || strcmp(command.c_str(), "R") == 0) {
				GetStandardCapabilities(pci_device_vector[select_pcie_device].bus, pci_device_vector[select_pcie_device].device, pci_device_vector[select_pcie_device].function);
				PrintCapability();
				cout << endl;
			}
			else if (strcmp(command.c_str(), "q") == 0 || strcmp(command.c_str(), "Q") == 0)
				break;
			else
				ErrorMessage(console, "Invalid input.");
		}
		catch (...) {
			ErrorMessage(console, "Invalid input.");
		}
	}

	UnloadPhyMemDriver();
	return EXIT_SUCCESS;
}

void Title(void) {
	cout << "======================================================================" << endl;
	cout << "     /$$$$$$$   /$$$$$$  /$$$$$$                 /$$ /$$   /$$" << endl;
	cout << "    | $$__  $$ /$$__  $$|_  $$_/                | $$|__/  | $$" << endl;
	cout << "    | $$  \\ $$| $$  \\__/  | $$    /$$$$$$   /$$$$$$$ /$$ /$$$$$$" << endl;
	cout << "    | $$$$$$$/| $$        | $$   /$$__  $$ /$$__  $$| $$|_  $$_/" << endl;
	cout << "    | $$____/ | $$        | $$  | $$$$$$$$| $$  | $$| $$  | $$" << endl;
	cout << "    | $$      | $$    $$  | $$  | $$_____/| $$  | $$| $$  | $$ /$$" << endl;
	cout << "    | $$      |  $$$$$$/ /$$$$$$|  $$$$$$$|  $$$$$$$| $$  |  $$$$/" << endl;
	cout << "    |__/       \\______/ |______/ \\_______/ \\_______/|__/   \\___/" << endl << endl;
	cout << "                                                  " << __DATE__ << " " << __TIME__ << endl;
	cout << "                                                          @KyuHyuk Lee" << endl;
	cout << "======================================================================" << endl << endl;
}

void Help(void) {
	cout << "h : Help" << endl;
	cout << "r : PCI-Compatible Configuration Registers" << endl;
	cout << "q : Exit" << endl << endl;
}

void FindPciDevice(void) {
	unsigned short index = 0;
	for (unsigned short bus = 0; bus <= 255; bus++) {
		for (unsigned short device = 0; device <= 31; device++) {
			for (unsigned short function = 0; function <= 7; function++) {
				DWORD data[2]; /* [0] : IO Port, [1] : Value */
				PciDevice pci_device;
				pci_device.index = index;
				pci_device.bus = bus;
				pci_device.device = device;
				pci_device.function = function;
				ReadPciDword(bus, device, function, 0x00, &data);
				pci_device.vendor_id = LOWORD(data[1]);
				pci_device.device_id = HIWORD(data[1]);
				ReadPciDword(bus, device, function, 0x08, &data);
				pci_device.class_code = data[1] >> 8;
				if (pci_device.vendor_id != 0x0000 && pci_device.device_id != 0x0000 && pci_device.vendor_id != 0xFFFF && pci_device.device_id != 0xFFFF) {
					pci_device_vector.push_back(pci_device);
					index++;
				}
			}
		}
	}
}

void GetPciDevice(void) {
	cout << "PCI Device List :" << endl;
	for (unsigned short index = 0; index < pci_device_vector.size(); index++) {
		cout << "[" << setfill(' ') << setw(2) << dec << pci_device_vector[index].index << "] ";
		cout << "Bus " << setfill('0') << setw(2) << right << uppercase << hex << pci_device_vector[index].bus << ", ";
		cout << "Device " << setfill('0') << setw(2) << right << uppercase << hex << pci_device_vector[index].device << ", ";
		cout << "Function" << setfill('0') << setw(2) << right << uppercase << hex << pci_device_vector[index].function << " - ";
		cout << GetVendorName(pci_device_vector[index].vendor_id) << ", ";
		cout << GetDeviceType(pci_device_vector[index].class_code);
		cout << " (Vendor ID : 0x" << setfill('0') << setw(4) << right << uppercase << hex << pci_device_vector[index].vendor_id << ", ";
		cout << " Device ID : 0x" << setfill('0') << setw(4) << right << uppercase << hex << pci_device_vector[index].device_id << ")" << endl;
	}
	cout << endl;
}

void SelectPciMessage(HANDLE console, PciDevice pci_device) {
	SetConsoleTextAttribute(console, 10);
	cout << "You selected \'Bus " << pci_device.bus << ", Device " << pci_device.device << ", Function " << pci_device.function << " - ";
	cout << GetVendorName(pci_device.vendor_id) << ", " << GetDeviceType(pci_device.class_code);
	cout << " (Vendor ID : 0x" << setfill('0') << setw(4) << right << uppercase << hex << pci_device.vendor_id << ", ";
	cout << " Device ID : 0x" << setfill('0') << setw(4) << right << uppercase << hex << pci_device.device_id << ")\'" << endl << endl;
	SetConsoleTextAttribute(console, 7);
}

void ErrorMessage(HANDLE console, string message) {
	SetConsoleTextAttribute(console, 12);
	cout << "[ERROR] " << message << endl << endl;
	SetConsoleTextAttribute(console, 7);
}

BOOL ExtractResource(uint16_t resource_id, LPCWSTR output_filename, LPCWSTR resource_type) {
	try {
		HRSRC resource_info = FindResource(nullptr, MAKEINTRESOURCE(resource_id), resource_type);
		if (resource_info == nullptr)
			return false;

		HGLOBAL resource_data = LoadResource(nullptr, resource_info);
		if (resource_data == nullptr)
			return false;

		void* file = LockResource(resource_data);
		if (file == nullptr)
			return false;

		uint32_t resource_size = SizeofResource(nullptr, resource_info);
		if (resource_size == 0)
			return false;

		HANDLE file_handle = CreateFile(output_filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		HANDLE file_map_handle = CreateFileMapping(file_handle, nullptr, PAGE_READWRITE, 0, resource_size, nullptr);
		if (file_map_handle == nullptr)
			return false;

		void* base_address = MapViewOfFile(file_map_handle, FILE_MAP_WRITE, 0, 0, 0);
		if (base_address == 0)
			return false;
		CopyMemory(base_address, file, resource_size);
		if (UnmapViewOfFile(base_address) == 0)
			return false;
		CloseHandle(file_map_handle);
		CloseHandle(file_handle);

		return true;
	}
	catch (...) {}
	return false;
}