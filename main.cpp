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
			else if (strcmp(command.c_str(), "r") == 0 || strcmp(command.c_str(), "R") == 0) {
				// PrintRegisters(bus, device, function);
				GetRegisters(bus, device, function);
				PrintType0ConfigurationSpaceHeader();
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
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                 [00h] Type 0 Configuration Space Header               |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|              Device ID            |             Vendor ID             |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 0, &data);
	printf("|                0x%04X             |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------|-----------------------------------|" << endl;
	cout << "|               Status              |              Command              |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 1, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------|-----------------------------------|" << endl;
	cout << "|             Class Code            |             Revision ID           |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 2, &data);
	printf("|              0x%06X             |                0x%02X               |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|    BIST    |   Header Type    |   Lantency Timer  |  Cache Line Size  |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 3, &data);
	printf("|    0x%02X    |       0x%02X       |        0x%02X       |        0x%02X       |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 0                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 4, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 1                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 5, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 2                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 6, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 3                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 7, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 4                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 8, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Base Address Register 5                       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 9, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                           Cardbus CIS Pointer                         |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 10, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|            Subsystem ID           |         Subsystem Vendor ID       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 11, &data);
	printf("|                0x%04X             |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                        Expansion ROM Base Address                     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 12, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|              Reserved             |        Capabilities Pointer       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 13, &data);
	printf("|              0x%06X             |                0x%02X               |\n", data[1] >> 8, LOBYTE(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                                Reserved                               |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 14, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|   Max Latency   |   Min Grant   |   Interrupt Pin  |  Interrupt Line  |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 15, &data);
	printf("|       0x%02X      |      0x%02X     |        0x%02X      |       0x%02X       |\n", HIBYTE(HIWORD(data[1])), LOBYTE(HIWORD(data[1])), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                 [40h] PCI Power Management Capability                 |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|   Power Management Capabilities   | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 16, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|         Data         |    Power Management Control/Status Register    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 17, &data);
	printf("|         0x%02X         |                    0x%06X                    |\n", HIBYTE(HIWORD(data[1])), LOWORD(data[1]) << 8 >> 8);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          [50h] MSI Capability                         |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|          Message Control          | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 20, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Message Lower Address                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 21, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Message Upper Address                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 22, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                                   |            Message Data           |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 23, &data);
	printf("|                                   |               0x%04X              |\n", LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         [70h] MSI-X Capability                        |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|          Message Control          | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 28, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                         Table Offset/Table BIR                        |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 29, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                           PBA Offset/PBA BIR                          |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 30, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      [80h] PCI Express Capability                     |" << endl;
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "| PCI Express Capabilities Register | Next Item Pointer | Capability ID |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 32, &data);
	printf("|               0x%04X              |        0x%02X       |      0x%02X     |\n", HIWORD(data[1]), HIBYTE(LOWORD(data[1])), LOBYTE(LOWORD(data[1])));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      Device Capabilities Register                     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 33, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|       Device Status Register      |      Device Control Register      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 34, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                       Link Capabilities Register                      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 35, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|        Link Status Register       |       Link Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 36, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                       Slot Capabilities Register                      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 37, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|        Slot Status Register       |       Slot Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 38, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|     Root Capabilities Register    |       Root Control Register       |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 39, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                          Root Status Register                         |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 40, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                     Device Capabilities 2 Register                    |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 41, &data);
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|      Device Status 2 Register     |     Device Control 2 Register     |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 42, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|                      Link Capabilities 2 Register                     |" << endl;
	printf("|                               0x%08X                              |\n", data[1]);
	cout << "|-----------------------------------------------------------------------|" << endl;
	cout << "|       Link Status 2 Register      |      Link Control 2 Register      |" << endl;
	ReadPciDword(bus, device, function, sizeof(DWORD) * 44, &data);
	printf("|               0x%04X              |               0x%04X              |\n", HIWORD(data[1]), LOWORD(data[1]));
	cout << "|-----------------------------------------------------------------------|" << endl << endl;

}

void GetRegisters(int bus, int device, int function) {
	GetCommonConfigurationSpace(bus, device, function);
	if (common_configuration_space.header_type == 0)
		GetType0ConfigurationSpaceHeader(bus, device, function);
	if (common_configuration_space.header_type == 1)
		GetType1ConfigurationSpaceHeader(bus, device, function);
}

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
	type_1_configuration_space_header.uo_base_upper = LOWORD(data[1]);
	type_1_configuration_space_header.io_base_limit = HIWORD(data[1]);
	ReadPciDword(bus, device, function, 0x38, &data);
	type_1_configuration_space_header.expansion_rom_base_address = data[1];
	ReadPciDword(bus, device, function, 0x3C, &data);
	type_1_configuration_space_header.bridge_control = HIWORD(data[1]);
	type_1_configuration_space_header.exists = TRUE;
}

void PrintType0ConfigurationSpaceHeader(void) {
	if (type_0_configuration_space_header.exists) {
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                [000h] Type 0 Configuration Space Header               |" << endl;
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|              Device ID            |             Vendor ID             |" << endl;
		printf("|                0x%04X             |               0x%04X              |\n", common_configuration_space.device_id, common_configuration_space.vendor_id);
		cout << "|-----------------------------------|-----------------------------------|" << endl;
		cout << "|               Status              |              Command              |" << endl;
		printf("|               0x%04X              |               0x%04X              |\n", common_configuration_space.status, common_configuration_space.command);
		cout << "|-----------------------------------|-----------------------------------|" << endl;
		cout << "|             Class Code            |             Revision ID           |" << endl;
		printf("|              0x%06X             |                0x%02X               |\n", common_configuration_space.class_code, common_configuration_space.revision_id);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|    BIST    |   Header Type    |   Lantency Timer  |  Cache Line Size  |" << endl;
		printf("|    0x%02X    |       0x%02X       |        0x%02X       |        0x%02X       |\n", common_configuration_space.bist, common_configuration_space.header_type, common_configuration_space.latency_timer, common_configuration_space.cache_line_size);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 0                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[0]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 1                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[1]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 2                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[2]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 3                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[3]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 4                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[4]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                         Base Address Register 5                       |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.base_address_register[5]);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                           Cardbus CIS Pointer                         |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.cardbus_cis_pointer);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|            Subsystem ID           |         Subsystem Vendor ID       |" << endl;
		printf("|                0x%04X             |               0x%04X              |\n", type_0_configuration_space_header.subsystem_id, type_0_configuration_space_header.subsystem_vendor_id);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                        Expansion ROM Base Address                     |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.expansion_rom_base_address);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|              Reserved             |        Capabilities Pointer       |" << endl;
		printf("|              0x%06X             |                0x%02X               |\n", type_0_configuration_space_header.reserved1, common_configuration_space.capabilities_pointer);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|                                Reserved                               |" << endl;
		printf("|                               0x%08X                              |\n", type_0_configuration_space_header.reserved2);
		cout << "|-----------------------------------------------------------------------|" << endl;
		cout << "|   Max Latency   |   Min Grant   |   Interrupt Pin  |  Interrupt Line  |" << endl;
		printf("|       0x%02X      |      0x%02X     |        0x%02X      |       0x%02X       |\n", type_0_configuration_space_header.max_latency, type_0_configuration_space_header.min_grant, common_configuration_space.interrupt_pin, common_configuration_space.interrupt_line);
		cout << "|-----------------------------------------------------------------------|" << endl << endl;
	}
}