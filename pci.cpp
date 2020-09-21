#include <Windows.h>
#include <winioctl.h>

#include "pci.h"

typedef struct PciDataDword {
	DWORD io_port;
	DWORD value;
} PciDataDword;

HANDLE driverHandle = INVALID_HANDLE_VALUE;

BOOL InstallDriver(LPWSTR pszDriverPath, LPWSTR pszDriverName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;

	//Remove any previous instance of the driver
	RemoveDriver(pszDriverName);

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		//Install the driver
		hService = CreateService(hSCManager,
			pszDriverName,
			pszDriverName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			pszDriverPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		CloseServiceHandle(hSCManager);

		if (hService == NULL)
			return FALSE;
	}
	else
		return FALSE;

	CloseServiceHandle(hService);

	return TRUE;
}

BOOL RemoveDriver(LPWSTR pszDriverName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	BOOL bResult;

	StopDriver(pszDriverName);

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = OpenService(hSCManager, pszDriverName, SERVICE_ALL_ACCESS);

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			bResult = DeleteService(hService);

			CloseServiceHandle(hService);
		}
		else
			return FALSE;
	}
	else
		return FALSE;
	return bResult;
}

BOOL StartDriver(LPWSTR pszDriverName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	BOOL bResult;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = OpenService(hSCManager, pszDriverName, SERVICE_ALL_ACCESS);

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			bResult = StartService(hService, 0, NULL);
			if (bResult == FALSE)
			{
				if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
					bResult = TRUE;
			}

			CloseServiceHandle(hService);
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return bResult;
}

BOOL StopDriver(LPWSTR pszDriverName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS ServiceStatus;
	BOOL bResult;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = OpenService(hSCManager, pszDriverName, SERVICE_ALL_ACCESS);

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			bResult = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);

			CloseServiceHandle(hService);
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return bResult;
}
//install and start driver

BOOL LoadPhyMemDriver()
{
	BOOL bResult;
	// CHAR szDriverPath[MAX_PATH];

	driverHandle = CreateFile(L"\\\\.\\rwdrv",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//If the driver is not running, install it
	if (driverHandle == INVALID_HANDLE_VALUE)
	{
		TCHAR programpath[MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, programpath);
		wcscat_s(programpath, L"\\RwDrv.sys");
		LPWSTR b = const_cast<LPTSTR>(TEXT("PHYMEM"));
		bResult = InstallDriver(programpath, b);

		if (!bResult)
			return FALSE;

		bResult = StartDriver(b);

		if (!bResult)
			return FALSE;

		driverHandle = CreateFile(L"\\\\.\\rwdrv",
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (driverHandle == INVALID_HANDLE_VALUE)
			return FALSE;
	}

	return TRUE;
}

//stop and remove driver
VOID UnloadPhyMemDriver()
{
	if (driverHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(driverHandle);
		driverHandle = INVALID_HANDLE_VALUE;
	}

	RemoveDriver(const_cast<LPTSTR>(TEXT("PHYMEM")));
}

BOOL ReadPciDword(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, PVOID pValue)
{
	BOOL bRet = FALSE;
	DWORD dwBytes;
	PciDataDword pp;//8 bytes
	// DWORD io_port;

	pp.io_port = 0xCF8;
	pp.value = (busNum << 16) | (devNum << 11) | (funcNum << 8) | (regOff & 0xFC) | 0x80000000;
	if (driverHandle != INVALID_HANDLE_VALUE)
	{
		/*  HANDLE       hDevice,
			DWORD        dwIoControlCode,
			LPVOID       lpInBuffer,
			DWORD        nInBufferSize,
			LPVOID       lpOutBuffer,
			DWORD        nOutBufferSize,
			LPDWORD      lpBytesReturned,
			LPOVERLAPPED lpOverlapped*/
		bRet = DeviceIoControl(driverHandle, 0x222824, &pp, sizeof(PciDataDword), pValue, 8, &dwBytes, NULL);//write dw
		pp.io_port = 0xCFC + (regOff & 0x03);
		pp.value = 0;
		if (bRet)
			bRet = DeviceIoControl(driverHandle, 0x222820, &pp, sizeof(PciDataDword), pValue, 8, &dwBytes, NULL);//read dw
	}

	if (bRet)
		return TRUE;
	else
		return FALSE;
}

//write pci configuration
BOOL WritePciDword(WORD busNum, WORD devNum, WORD funcNum, WORD regOff, DWORD Value)
{
	BOOL bRet = FALSE;
	DWORD dwBytes;
	PciDataDword pp;//8 bytes

	pp.io_port = 0xCF8;
	pp.value = (busNum << 16) | (devNum << 11) | (funcNum << 8) | (regOff & 0xFC) | 0x80000000;

	if (driverHandle != INVALID_HANDLE_VALUE)
	{
		bRet = DeviceIoControl(driverHandle, 0x222824, &pp, sizeof(PciDataDword), &pp, sizeof(PciDataDword), &dwBytes, NULL);//write dw
		pp.io_port = 0xCFC + (regOff & 0x03);
		pp.value = Value;
		if (bRet)
			bRet = DeviceIoControl(driverHandle, 0x222824, &pp, sizeof(PciDataDword), &pp, sizeof(PciDataDword), &dwBytes, NULL);//write dw
	}

	if (bRet)
		return TRUE;
	else
		return FALSE;
}