#include <Windows.h>
#include <iostream>
#include <string>

#include "main.h"
#include "pci.h"
#include "resource.h"

using namespace std;

int main() {
	int bus = -1, device = -1, function = -1;
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
	do {
		cout << "[PCIedit] >> ";
		cin >> bus >> device >> function;
		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			ErrorMessage(console, "Input Example : <Bus> <Device> <Function>");
		}
	} while (bus < 0 || device < 0 || function < 0);
	SelectPciMessage(console, bus, device, function);

	/* PCIedit Colsone */
	while (TRUE) {
		//WritePCIDW(bus, dev, func, 0, 0x12345678);

		cout << "[PCIedit] >> ";
		cin >> command;
		try {
			if (strcmp(command.c_str(), "h") == 0 || strcmp(command.c_str(), "H") == 0)
				Help();
			else if (strcmp(command.c_str(), "r") == 0 || strcmp(command.c_str(), "R") == 0)
				PrintRegisters(bus, device, function);
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
	cout << ">> Select PCI Device. Enter BUS, Device, Function." << endl << endl;
}

void Help(void) {
	cout << "h : Help" << endl;
	cout << "r : PCI-Compatible Configuration Registers" << endl;
	cout << "q : Exit" << endl << endl;
}

void SelectPciMessage(HANDLE console, int bus, int device, int function) {
	SetConsoleTextAttribute(console, 10);
	cout << "You selected Bus " << bus << ", Device " << device << ", Function " << function << endl << endl;
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

void PrintRegisters(int bus, int device, int function) {
	DWORD data[2]; /* [0] : IO Port, [1] : Value */
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|          [00h] Type 0 Configuration Space Header         |" << endl;
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|          Device ID         |          Vendor ID          |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 0, &data);
	printf("|            0x%04X          |            0x%04X           |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|----------------------------|-----------------------------|" << endl;
	cout << "|           Status           |           Command           |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 1, &data);
	printf("|            0x%04X          |            0x%04X           |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|----------------------------|-----------------------------|" << endl;
	cout << "|         Class Code         |          Revision ID        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 2, &data);
	printf("|          0x%06X          |             0x%02X            |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|  BIST  | Header Type  | Lantency Timer | Cache Line Size |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 3, &data);
	printf ("|  0x%02X  |     0x%02X     |      0x%02X      |       0x%02X      |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 0                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 4, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 1                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 5, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 2                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 6, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 3                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 7, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 4                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 8, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                  Base Address Register 5                 |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 9, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                    Cardbus CIS Pointer                   |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 10, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|        Subsystem ID        |      Subsystem Vendor ID    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 11, &data);
	printf("|            0x%04X          |            0x%04X           |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                 Expansion ROM Base Address               |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 12, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|          Reserved          |     Capabilities Pointer    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 13, &data);
	printf("|          0x%06X          |             0x%02X            |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|----------------------------------------------------------|" << endl;
	cout << "|                          Reserved                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 14, &data);
	printf("|                         0x%08X                       |\n", data[1]);
	cout << "|----------------------------------------------------------|" << endl;
	cout << "| Max Latency | Min Grant | Interrupt Pin | Interrupt Line |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 15, &data);
	printf("|     0x%02X    |    0x%02X   |      0x%02X     |      0x%02X      |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|----------------------------------------------------------|" << endl;
}