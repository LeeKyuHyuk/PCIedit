#pragma once

typedef struct CommonConfigurationSpace {
	BOOL exists;
	WORD vendor_id;
	WORD device_id;
	WORD command;
	WORD status;
	BYTE revision_id;
	DWORD class_code;
	BYTE cache_line_size;
	BYTE latency_timer;
	BYTE header_type;
	BYTE bist;
	BYTE capabilities_pointer;
	BYTE interrupt_line;
	BYTE interrupt_pin;
} CommonConfigurationSpace;

typedef struct Type0ConfigurationSpaceHeader {
	BOOL exists;
	DWORD base_address_register[6];
	DWORD cardbus_cis_pointer;
	WORD subsystem_vendor_id;
	WORD subsystem_id;
	DWORD expansion_rom_base_address;
	DWORD reserved1;
	DWORD reserved2;
	BYTE min_grant;
	BYTE max_latency;
} Type0ConfigurationSpaceHeader;

typedef struct Type1ConfigurationSpaceHeader {
	BOOL exists;
	DWORD base_address_register[2];
	BYTE primary_bus_number;
	BYTE secondary_bus_number;
	BYTE subordinate_bus_number;
	BYTE secondary_latency_timer;
	BYTE io_base;
	BYTE io_limit;
	WORD secondary_status;
	WORD memory_base;
	WORD memory_limit;
	WORD prefetchable_memory_base;
	WORD prefetchable_memory_limit;
	DWORD prefetchable_memory_base_upper;
	DWORD prefetchable_memory_limit_upper;
	WORD io_base_upper;
	WORD io_limit_upper;
	DWORD expansion_rom_base_address;
	WORD bridge_control;
} Type1ConfigurationSpaceHeader;

void GetCommonConfigurationSpace(int bus, int device, int function);
void GetType0ConfigurationSpaceHeader(int bus, int device, int function);
void GetType1ConfigurationSpaceHeader(int bus, int device, int function);
void PrintType0ConfigurationSpaceHeader(void);
void PrintType1ConfigurationSpaceHeader(void);