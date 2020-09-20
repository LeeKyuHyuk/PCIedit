#pragma once

BOOL InstallDriver(LPWSTR pszDriverPath, LPWSTR pszDriverName);
BOOL RemoveDriver(LPWSTR pszDriverName);
BOOL StartDriver(LPWSTR pszDriverName);
BOOL StopDriver(LPWSTR pszDriverName);

//driver initialize
BOOL LoadPhyMemDriver();
VOID UnloadPhyMemDriver();

//access PCI bus
BOOL ReadPCIDW(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, PVOID pValue);
BOOL WritePCIDW(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, DWORD Value);