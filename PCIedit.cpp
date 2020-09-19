#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "resource.h"
#include "pci.h"

using namespace std;

BOOL ExtractResource(uint16_t ResourceID, LPCWSTR OutputFileName, LPCWSTR ResType)
{
    try
    {
        HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(ResourceID), ResType);
        if (hResource == nullptr)
        {
            return false;
        }

        HGLOBAL hFileResource = LoadResource(nullptr, hResource);
        if (hFileResource == nullptr)
        {
            return false;
        }

        void* lpFile = LockResource(hFileResource);
        if (lpFile == nullptr)
        {
            return false;
        }

        std::uint32_t dwSize = SizeofResource(nullptr, hResource);
        if (dwSize == 0)
        {
            return false;
        }

        HANDLE hFile = CreateFile(OutputFileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        HANDLE hFilemap = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, dwSize, nullptr);
        if (hFilemap == nullptr)
        {
            return false;
        }

        void* lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);
        if (lpBaseAddress == 0)
            return false;
        CopyMemory(lpBaseAddress, lpFile, dwSize);
        if (UnmapViewOfFile(lpBaseAddress) == 0)
            return false;
        CloseHandle(hFilemap);
        CloseHandle(hFile);

        return true;
    }
    catch (...) {}
    return false;
}

int main()
{
    ExtractResource(IDR_BINARY1, L"RwDrv.sys", L"BINARY");

    BOOL b = LoadPhyMemDriver();
    //BYTE bb=LoadPhyMemDriverMY();printf("Num=  %d\n",bb);

    if (b == FALSE)
    {
        printf("load RwDrv.sys failed\n");
        system("PAUSE");
        exit(-1);
    }


    int bus = 0;
    int dev = 0;
    int func = 0;
    printf("BUS : ");
    scanf_s("%d", &bus);
    printf("Device : ");
    scanf_s("%d", &dev);
    printf("Function : ");
    scanf_s("%d", &func);
    DWORD dev_ven[2];
    //bus,dev,func,offset
    ReadPCIDW(bus, dev, func, 0, &dev_ven);
    //printf("Bus %d, Device %d, Function %d, Vendor ID 0x%x, Device ID 0x%x\n",	bus, dev, func, LOWORD(dev_ven[0]), HIWORD(dev_ven[0]));
    printf("Bus %d, Device %d, Function %d, Vendor ID 0x%x, Device ID 0x%x\n", bus, dev, func, LOWORD(dev_ven[1]), HIWORD(dev_ven[1]));
    //WritePCIDW(bus, dev, func, 0, 0x12345678);

    //getchar();
    UnloadPhyMemDriver();
    system("PAUSE");
    return 0;
}