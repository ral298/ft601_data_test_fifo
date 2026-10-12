#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ftd3xx.h"
#include "Types.h"

static const char * statusString (FT_STATUS status)
{
    switch (status)
    {
        case FT_OK:
            return "OK";
        case FT_INVALID_HANDLE:
            return "INVALID_HANDLE";
        case FT_DEVICE_NOT_FOUND:
            return "DEVICE_NOT_FOUND";
        case FT_DEVICE_NOT_OPENED:
            return "DEVICE_NOT_OPENED";
        case FT_IO_ERROR:
            return "IO_ERROR";
        case FT_INSUFFICIENT_RESOURCES:
            return "INSUFFICIENT_RESOURCES";
        case FT_INVALID_PARAMETER:
            return "INVALID_PARAMETER";
        case FT_INVALID_BAUD_RATE:
            return "INVALID_BAUD_RATE";
        case FT_DEVICE_NOT_OPENED_FOR_ERASE:
            return "DEVICE_NOT_OPENED_FOR_ERASE";
        case FT_DEVICE_NOT_OPENED_FOR_WRITE:
            return "DEVICE_NOT_OPENED_FOR_WRITE";
        case FT_FAILED_TO_WRITE_DEVICE:
            return "FAILED_TO_WRITE_DEVICE";
        case FT_MEM_INSUFFICIENT_ERROR:
            return "MEM_INSUFFICIENT_ERROR";
        case FT_OVERFLOW_ERROR:
            return "OVERFLOW_ERROR";

        case FT_INVALID_ARGS:
            return "INVALID_ARGS";
        case FT_NOT_SUPPORTED:
            return "NOT_SUPPORTED";
        case FT_NO_MORE_ITEMS:
            return "NO_MORE_ITEMS";
        case FT_TIMEOUT:
            return "TIMEOUT";
        case FT_OPERATION_ABORTED:
            return "OPERATION_ABORTED";
        case FT_RESERVED_PIPE:
            return "RESERVED_PIPE";
        case FT_INVALID_CONTROL_REQUEST_DIRECTION:
            return "INVALID_CONTROL_REQUEST_DIRECTION";
        case FT_INVALID_CONTROL_REQUEST_TYPE:
            return "INVALID_CONTROL_REQUEST_TYPE";
        case FT_IO_PENDING:
            return "IO_PENDING";
        case FT_IO_INCOMPLETE:
            return "IO_INCOMPLETE";
        case FT_HANDLE_EOF:
            return "HANDLE_EOF";
        case FT_BUSY:
            return "BUSY";
        case FT_NO_SYSTEM_RESOURCES:
            return "NO_SYSTEM_RESOURCES";
        case FT_DEVICE_LIST_NOT_READY:
            return "DEVICE_LIST_NOT_READY";
        case FT_OTHER_ERROR:
            return "OTHER_ERROR";
        default:
            return "UNKNOWN ERROR";
    }
}


static
FT_STATUS testDeviceIndex(DWORD index)
{
    FT_STATUS ftStatus;
    DWORD     detailFlags = 0;
    DWORD     detailType = 0;
    DWORD     detailID = 0;
    DWORD     detailLocId = 0;
    char      detailSerialNumber[16] = {0};
    char      detailDescription[32] = {0};
    FT_HANDLE detailHandle = NULL;
    FT_HANDLE testHandle = NULL;
    
    ftStatus = FT_GetDeviceInfoDetail(
                   index,
                   &detailFlags,
                   &detailType,
                   &detailID,
                   &detailLocId,
                   detailSerialNumber,
                   detailDescription,
                   &detailHandle);
    if (ftStatus != FT_OK) 
    {
        printf("FT_GetDeviceInfoDetail failed: %s.\n", 
               statusString(ftStatus));
        return ftStatus;
    }
    
    printf("Flags: %08X, Type: %08X, ID: %08X, LocId: %08X "
           "SN: %s, PD: %s, Handle: %p\n",
           detailFlags,
           detailType,
           detailID,
           detailLocId,
           detailSerialNumber,
           detailDescription,
           detailHandle);

    // Test OPEN_BY_INDEX
    testHandle = NULL;    
    ftStatus = FT_Create(
                   (PVOID)(uintptr_t)index,
                   FT_OPEN_BY_INDEX,
                   &testHandle);
    if (ftStatus != FT_OK) 
    {
        printf("FT_Create (OPEN_BY_INDEX %d) failed: %s.\n", 
               (int)index,
               statusString(ftStatus));
        return ftStatus;
    }
    
    (void)FT_Close(testHandle);

    // Test OPEN_BY_DESCRIPTION
    if (strlen(detailDescription) == 0)
    {
        printf("Skipping FT_OPEN_BY_DESCRIPTION test as "
               "product description is absent.\n");
    }
    else
    {
        testHandle = NULL;        
        ftStatus = FT_Create(
                       (PVOID)detailDescription,
                       FT_OPEN_BY_DESCRIPTION,
                       &testHandle);
        if (ftStatus != FT_OK) 
        {
            printf("FT_Create (OPEN_BY_DESCRIPTION '%s') failed: %s.\n", 
                   detailDescription,
                   statusString(ftStatus));
            return ftStatus;
        }
        
        (void)FT_Close(testHandle);
    }

    // Test FT_OPEN_BY_SERIAL_NUMBER
    if (strlen(detailSerialNumber) == 0)
    {
        printf("Skipping FT_OPEN_BY_SERIAL_NUMBER test as "
               "serial number is absent.\n");
    }
    else
    {
        testHandle = NULL;        
        ftStatus = FT_Create(
                       (PVOID)detailSerialNumber,
                       FT_OPEN_BY_SERIAL_NUMBER,
                       &testHandle);
        if (ftStatus != FT_OK) 
        {
            printf("FT_Create (FT_OPEN_BY_SERIAL_NUMBER '%s') failed: %s.\n", 
                   detailSerialNumber,
                   statusString(ftStatus));
            return ftStatus;
        }
    
        (void)FT_Close(testHandle);
    }
    
    return FT_OK;
}
int main()
{
	FT_HANDLE ftHandle = NULL;
    FT_STATUS ftStatus;
    USHORT vendorId;
    USHORT productId;

    FT_DEVICE_LIST_INFO_NODE *devInfo = NULL;
    DWORD                     numDevs = 0;
    int                       i;
    
    printf("\nTesting DeviceInfoList...\n");
    
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK) 
    {
        printf("FT_CreateDeviceInfoList failed: %s\n", 
               statusString(ftStatus));
        goto exit;
    }
    
    if (numDevs == 0)
    {
        printf("No devices connected.\n");
        goto exit;
    }

    /* Allocate storage */
    devInfo = calloc((size_t)numDevs,
                     sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL)
    {
        printf("Allocation failure.\n");
        goto exit;
    }
    
    /* Populate the list of info nodes */
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    if (ftStatus != FT_OK)
    {
        printf("FT_GetDeviceInfoList failed: %s\n",
               statusString(ftStatus));
        goto exit;
    }
    printf("%d devices connected.\n",numDevs);

    for (i = 0; i < (int)numDevs; i++) 
    {
        printf("Device %d:\n",i);
        printf("  Flags = 0x%x\n", (unsigned int)devInfo[i].Flags);
        printf("  Type = 0x%x\n", (unsigned int)devInfo[i].Type);
        printf("  ID = 0x%08x\n", (unsigned int)devInfo[i].ID);
        printf("  LocId = 0x%x\n", (unsigned int)devInfo[i].LocId);
        printf("  SerialNumber = %s\n", devInfo[i].SerialNumber);
        printf("  Description = %s\n", devInfo[i].Description);
        printf("  ftHandle = %p\n", devInfo[i].ftHandle);

        ftStatus = testDeviceIndex((DWORD)i);
        if (ftStatus != FT_OK)
        {
            goto exit;
        }
    }


    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX,&ftHandle);
    if (FT_FAILED(ftStatus))
    {
        (void)printf("FT_Create failed %d\n",ftStatus);
        return 0;
    }

    ftStatus = FT_GetVIDPID(ftHandle,&vendorId,&productId);
    if (ftStatus != FT_OK)
    {
        (void)printf("FT_GetVIDPID failed %d\n",ftStatus);
    }
    (void)printf("\tVendor ID: 0x%04X\n\tProduct ID: 0x%04X\n",vendorId, productId);
exit:
    return 0;
}