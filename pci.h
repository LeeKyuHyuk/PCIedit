#include <Windows.h>

typedef struct tagPHYMEM_IOPR
{
	DWORD io_port;		//
	DWORD value;		//
} PHYMEM_IOPR, * PPHYMEM_IOPR;

//driver initialize
BOOL LoadPhyMemDriver();
BYTE LoadPhyMemDriverMY();
VOID UnloadPhyMemDriver();

//access PCI bus
BOOL ReadPCIDW(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, PVOID pValue);
BOOL WritePCIDW(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, DWORD Value);