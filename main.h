#pragma once

void Title(void);
void Help(void);
void SelectPciMessage(HANDLE console, int bus, int device, int function);
void ErrorMessage(HANDLE console, std::string message);
BOOL ExtractResource(uint16_t resource_id, LPCWSTR output_filename, LPCWSTR resource_type);
