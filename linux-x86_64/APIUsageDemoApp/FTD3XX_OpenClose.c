#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"

#define FTDI_VID                        0x0403
#define FTDI_PID_600                    0x601E
#define FTDI_PID_601                    0x601F
#define FTDI_CURRENT_FIRMWARE_VERSION   0x0109

///////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////////////////////////////
bool ValidateDevice(FT_HANDLE ftHandle);
unsigned int GetNumberOfDevicesConnected();
bool GetDeviceSerialNumber(unsigned int dwDeviceIndex, char* SerialNumber);
bool GetDeviceDescription(unsigned int dwDeviceIndex, char* ProductDescription);
bool GetDevicesSerialNumbers(char** SerialNumbers);
bool GetDevicesProductDescriptions(char** ProductDescriptions);
unsigned int GetDevicesInfoList(FT_DEVICE_LIST_INFO_NODE **pptDevicesInfo);
unsigned int GetDeviceInfoDetail(unsigned int dwDeviceIndex, unsigned int* Flags, unsigned int* Type, unsigned int *ID, char* SerialNumber, char* Description, FT_HANDLE* Handle);
void DisplayDeviceInfoDetail(unsigned int dwNumDevices);
void DisplayDeviceInfoList();
void DisplaySerialNumbersOfDevicesConnected(unsigned int dwNumDevices);
void DisplayNumberOfDevicesConnected();
void DisplayProductDescriptionOfDevicesConnected(unsigned int dwNumDevices);

///////////////////////////////////////////////////////////////////////////////////
// OpenCloseTest Function
///////////////////////////////////////////////////////////////////////////////////
BOOL OpenCloseTest(void)
{
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle = NULL;
    unsigned int dwNumDevicesConnected = 0;

    // Display information of devices connected
    DisplayDeviceInfoList();

    // Get the number of devices connected
    dwNumDevicesConnected = GetNumberOfDevicesConnected();
    if (dwNumDevicesConnected == 0)
    {
        CMD_LOG("\n\tNo device is connected to the machine! Please connect a device.\n");
        return false;
    }
    else if (dwNumDevicesConnected > 1)
    {
        CMD_LOG("\n\t%d devices are connected to the machine! Please ensure only 1 device is connected.\n", dwNumDevicesConnected);
        return false;
    }

    // Open a device by SERIAL NUMBER
    CMD_LOG("\n\tOpen by SERIAL NUMBER.\n");
    char SerialNumber[16] = { 0 };
    int device_index = 0;
    if (!GetDeviceSerialNumber(device_index, SerialNumber))
    {
        CMD_LOG("Error: Failed to get Serial Number.\n");
        return false;
    }

    ftStatus = FT_Create(SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to create device with serial number %s, FT_Status = %d\n", SerialNumber, ftStatus);
        return false;
    }
    if (!ValidateDevice(ftHandle))
    {
        CMD_LOG("Error: Validation failed for device with serial number %s.\n", SerialNumber);
        FT_Close(ftHandle);
        return false;
    }
    ftStatus = FT_Close(ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to close device handle, FT_Status = %d\n", ftStatus);
        return false;
    }

    // Open a device by DESCRIPTION
    CMD_LOG("\n\tOpen by DESCRIPTION.\n");
    char Description[32] = { 0 };
    if (!GetDeviceDescription(0, Description))
    {
        CMD_LOG("Error: Failed to get Product Description.\n");
        return false;
    }

    ftStatus = FT_Create(Description, FT_OPEN_BY_DESCRIPTION, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to create device with description %s, FT_Status = %d\n", Description, ftStatus);
        return false;
    }
    if (!ValidateDevice(ftHandle))
    {
        CMD_LOG("Error: Validation failed for device with description %s.\n", Description);
        FT_Close(ftHandle);
        return false;
    }
    FT_Close(ftHandle);

    // Open a device by INDEX
    CMD_LOG("\n\tOpen by INDEX.\n");
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to create device by index, FT_Status = %d\n", ftStatus);
        return false;
    }
    if (!ValidateDevice(ftHandle))
    {
        CMD_LOG("Error: Validation failed for device by index.\n");
        FT_Close(ftHandle);
        return false;
    }
    FT_Close(ftHandle);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////
// Helper Functions
///////////////////////////////////////////////////////////////////////////////////

// Gets the number of devices connected to the machine using FT_ListDevices
unsigned int GetNumberOfDevicesConnected()
{
    FT_STATUS ftStatus = FT_OK;
    unsigned int dwNumDevices = 0;

    ftStatus = FT_ListDevices(&dwNumDevices, NULL, FT_LIST_NUMBER_ONLY);
    if (FT_FAILED(ftStatus))
    {
        return 0;
    }

    return dwNumDevices;
}

// Gets the device serial number by index
bool GetDeviceSerialNumber(unsigned int dwDeviceIndex, char* SerialNumber)
{
    FT_STATUS ftStatus = FT_OK;
    void* pDeviceIndex = (void*)(uintptr_t)dwDeviceIndex;

    ftStatus = FT_ListDevices(pDeviceIndex, SerialNumber, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_ListDevices Failed for SerialNumber, FT_Status=%d, Error Code=%d\n", ftStatus, ftStatus);
        return false;
    }
    CMD_LOG("FT_ListDevices Succeeded, SerialNumber=%s\n", SerialNumber);
    return true;
}

// Gets the device description by index
bool GetDeviceDescription(unsigned int dwDeviceIndex, char* ProductDescription)
{
    FT_STATUS ftStatus = FT_OK;
    void* pDeviceIndex = (void*)(uintptr_t)dwDeviceIndex;

    ftStatus = FT_ListDevices(pDeviceIndex, ProductDescription, FT_LIST_BY_INDEX | FT_OPEN_BY_DESCRIPTION);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_ListDevices Failed for ProductDescription, FT_Status=%d, Error Code=%d\n", ftStatus, ftStatus);
        return false;
    }
    CMD_LOG("FT_ListDevices Succeeded, ProductDescription=%s\n", ProductDescription);
    return true;
}

// Gets the serial numbers of all devices connected
bool GetDevicesSerialNumbers(char** SerialNumbers)
{
    FT_STATUS ftStatus = FT_OK;
    unsigned int dwNumDevices = 0;

    ftStatus = FT_ListDevices(SerialNumbers, &dwNumDevices, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_ListDevices Failed for all SerialNumbers, FT_Status=%d, Error Code=%d\n", ftStatus, ftStatus);
        return false;
    }
    CMD_LOG("FT_ListDevices Succeeded, dwNumDevices=%d\n", dwNumDevices);
    return true;
}

// Gets the product descriptions of all devices connected
bool GetDevicesProductDescriptions(char** ProductDescriptions)
{
    FT_STATUS ftStatus = FT_OK;
    unsigned int dwNumDevices = 0;

    ftStatus = FT_ListDevices(ProductDescriptions, &dwNumDevices, FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_ListDevices Failed for all ProductDescriptions, FT_Status=%d, Error Code=%d\n", ftStatus, ftStatus);
        return false;
    }
    CMD_LOG("FT_ListDevices Succeeded, dwNumDevices=%d\n", dwNumDevices);
    return true;
}

// Displays information of all devices connected
void DisplayDeviceInfoList()
{
    FT_DEVICE_LIST_INFO_NODE *ptDeviceInfo = NULL;
    unsigned int dwNumDevices = 0;

    dwNumDevices = GetDevicesInfoList(&ptDeviceInfo);
    if (dwNumDevices == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < dwNumDevices; i++)
    {
        CMD_LOG("\n\tFT_GetDeviceInfoList:\n\tDevice[%d]\n", i);
        CMD_LOG("\t\tFlags: 0x%x [%s]\n", ptDeviceInfo[i].Flags,
            ptDeviceInfo[i].Flags & FT_FLAGS_SUPERSPEED ? "USB 3" : 
            ptDeviceInfo[i].Flags & FT_FLAGS_HISPEED ? "USB 2" : "");
        CMD_LOG("\t\tType: %d\n", ptDeviceInfo[i].Type);
        CMD_LOG("\t\tID: 0x%08X\n", ptDeviceInfo[i].ID);
        CMD_LOG("\t\tLocId = 0x%x\n", (unsigned int)ptDeviceInfo[i].LocId);
        if (ptDeviceInfo[i].Flags & FT_FLAGS_OPENED)
        {
            CMD_LOG("\t\tftHandle: 0x%p [Device is being used by another process]\n", ptDeviceInfo[i].ftHandle);
        }
        CMD_LOG("\t\tSerialNumber: %s\n", ptDeviceInfo[i].SerialNumber);
        CMD_LOG("\t\tDescription: %s\n", ptDeviceInfo[i].Description);
    }
}

// Gets the list of device info for all devices connected
unsigned int GetDevicesInfoList(FT_DEVICE_LIST_INFO_NODE **pptDevicesInfo)
{
    FT_STATUS ftStatus = FT_OK;
    unsigned int dwNumDevices = 0;

    ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
    if (FT_FAILED(ftStatus))
    {
        return 0;
    }

    *pptDevicesInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * dwNumDevices);
    if (!(*pptDevicesInfo))
    {
        return 0;
    }

    ftStatus = FT_GetDeviceInfoList(*pptDevicesInfo, &dwNumDevices);
    if (FT_FAILED(ftStatus))
    {
        free(*pptDevicesInfo);
        *pptDevicesInfo = NULL;
        return 0;
    }

    return dwNumDevices;
}

// Validate device with error handling
bool ValidateDevice(FT_HANDLE ftHandle)
{
    FT_STATUS ftStatus = FT_OK;
    unsigned short uwVID = 0;
    unsigned short uwPID = 0;

    // Get and check device VID and PID
    ftStatus = FT_GetVIDPID(ftHandle, &uwVID, &uwPID);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to get VID/PID, FT_Status = %d\n", ftStatus);
        return false;
    }
    CMD_LOG("\t\tVendor ID: 0x%04X\n", uwVID);
    CMD_LOG("\t\tProduct ID: 0x%04X\n", uwPID);

    // Get firmware version
    unsigned int ulFirmwareVersion = 0;
    ftStatus = FT_GetFirmwareVersion(ftHandle, &ulFirmwareVersion);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to get firmware version, FT_Status = %d\n", ftStatus);
        return false;
    }
    CMD_LOG("\t\tFirmware version: 0x%04x\n", ulFirmwareVersion);
    if (ulFirmwareVersion != FTDI_CURRENT_FIRMWARE_VERSION)
    {
        CMD_LOG("\t\tWARNING! Expected firmware version: 0x%04x\n", FTDI_CURRENT_FIRMWARE_VERSION);
    }

    // Get driver version
    unsigned int ulDriverVersion = 0;
    ftStatus = FT_GetDriverVersion(ftHandle, &ulDriverVersion);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to get driver version, FT_Status = %d\n", ftStatus);
        return false;
    }
    CMD_LOG("\t\tDriver version: 0x%04x\n", ulDriverVersion);

    // Get library version
    unsigned int ulLibraryVersion = 0;
    ftStatus = FT_GetLibraryVersion(&ulLibraryVersion);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("Error: Failed to get library version, FT_Status = %d\n", ftStatus);
        return false;
    }
    CMD_LOG("\t\tLibrary version: 0x%04x\n", ulLibraryVersion);

    return true;
}

