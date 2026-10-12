/*
 * FT600 API Usage Demo App
 *
 * Copyright (C) 2015 FTDI Chip
 * FTD3XX_Descriptors.h
 *
 * Converted to Linux C code.
 */

#include <stdio.h>
#include <stdbool.h>
#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"

// // CMD_LOG is a macro for printing output to the console.
// // This is a standard C equivalent for what the original code was doing.
// #define CMD_LOG(...) printf(__VA_ARGS__)

// Function prototype for displaying descriptor information.
// Changed VOID to void and PFT_COMMON_DESCRIPTOR to const PFT_COMMON_DESCRIPTOR.
static void DisplayDescriptor(FT_HANDLE a_FTHandle, const PFT_COMMON_DESCRIPTOR pDescriptor);

///////////////////////////////////////////////////////////////////////////////////
// Demonstrates querying of USB descriptors:
// FT_GetDeviceDescriptor          Queries the USB Device descriptor
// FT_GetConfigurationDescriptor   Queries the USB Configuration descriptor
// FT_GetInterfaceDescriptor       Queries the USB Interface descriptors
//                                   There are 2 interfaces.
//                                   1st interface is reserved. user should not touch.
//                                   2nd interface interface is for user.
// FT_GetPipeInformation           Queries the USB Pipe information; 
//                                   There are 2 pipes for 1st interface.
//                                   There are 8 pipes for the 2nd interface by default.
// FT_GetDescriptor                Queries a USB descriptor
///////////////////////////////////////////////////////////////////////////////////
BOOL DescriptorTest(void)
{
    // Replaced BOOL with standard C++ bool
    FT_DEVICE_DESCRIPTOR DeviceDescriptor = {0};
    FT_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor = {0};
    FT_INTERFACE_DESCRIPTOR InterfaceDescriptor = {0};
    FT_PIPE_INFORMATION Pipe;
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle;

    // Open a device
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_Create failed with status: %d\n", ftStatus);
        return (ftStatus == FT_OK);
    }

    // Get device descriptor
    ftStatus = FT_GetDeviceDescriptor(ftHandle, &DeviceDescriptor);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_GetDeviceDescriptor failed with status: %d\n", ftStatus);
        FT_Close(ftHandle);
        return (ftStatus == FT_OK);
    }
    DisplayDescriptor(ftHandle, (PFT_COMMON_DESCRIPTOR)&DeviceDescriptor);

    // Get configuration descriptor
    ftStatus = FT_GetConfigurationDescriptor(ftHandle, &ConfigurationDescriptor);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_GetConfigurationDescriptor failed with status: %d\n", ftStatus);
        FT_Close(ftHandle);
        return (ftStatus == FT_OK);
    }
    DisplayDescriptor(ftHandle, (PFT_COMMON_DESCRIPTOR)&ConfigurationDescriptor);

    if (ConfigurationDescriptor.bNumInterfaces != 2)
    {
        CMD_LOG("Unexpected number of interfaces: %d\n", ConfigurationDescriptor.bNumInterfaces);
        FT_Close(ftHandle);
        return (ftStatus == FT_OK);
    }

    // Iterate the interfaces
    for (int j=0; j<ConfigurationDescriptor.bNumInterfaces; j++)
    {
        // Get interface descriptor
        ftStatus = FT_GetInterfaceDescriptor(ftHandle, j, &InterfaceDescriptor);
        if (FT_FAILED(ftStatus))
        {
            CMD_LOG("FT_GetInterfaceDescriptor failed with status: %d\n", ftStatus);
            FT_Close(ftHandle);
            return (ftStatus == FT_OK);
        }
        DisplayDescriptor(ftHandle, (PFT_COMMON_DESCRIPTOR)&InterfaceDescriptor);

        // Iterate the endpoints of the interface
        for (int i=0; i<InterfaceDescriptor.bNumEndpoints; i++)
        {
            // Get pipe information
            ftStatus = FT_GetPipeInformation(ftHandle, j, i, &Pipe);
            if (FT_FAILED(ftStatus))
            {
                CMD_LOG("FT_GetPipeInformation failed with status: %d\n", ftStatus);
                FT_Close(ftHandle);
                return (ftStatus == FT_OK);
            }

            CMD_LOG("\n");
            CMD_LOG("\t\tPIPE INFORMATION\n");
            CMD_LOG("\t\t0x%02x\n"        , Pipe.PipeId);
            CMD_LOG("\t\t0x%02x (%s)\n"    , Pipe.PipeType,
                    FT_IS_INTERRUPT_PIPE(Pipe.PipeType) ? "INTERRUPT" : "BULK");
            CMD_LOG("\t\t0x%04x (%d)\n"    , Pipe.MaximumPacketSize, Pipe.MaximumPacketSize);
            CMD_LOG("\t\t0x%02x\n"        , Pipe.Interval);
        }
    }

    // Close device handle
    FT_Close(ftHandle);
    return (ftStatus == FT_OK);
}


static void DisplayDescriptor(FT_HANDLE a_FTHandle, const PFT_COMMON_DESCRIPTOR pDescriptor)
{
    if (!pDescriptor)
    {
        return;
    }

    FT_STATUS ftResult = FT_OK;
    ULONG ulBytesTransferred = 0;

    switch (pDescriptor->bDescriptorType)
    {
        //--------------------------------------------------------------------------------------------//
        // USB 1.1 descriptors
        //--------------------------------------------------------------------------------------------//

        case FT_DEVICE_DESCRIPTOR_TYPE:
        {
            PFT_DEVICE_DESCRIPTOR pDeviceDesc = (PFT_DEVICE_DESCRIPTOR)pDescriptor;

            // Replaced FT_STRING_DESCRIPTOR with its standard C equivalent
            FT_STRING_DESCRIPTOR StringDescriptorManufacturer = {0};
            FT_STRING_DESCRIPTOR StringDescriptorProduct = {0};
            FT_STRING_DESCRIPTOR StringSerialNumber = {0};

            ftResult = FT_GetDescriptor(a_FTHandle,
                                        FT_STRING_DESCRIPTOR_TYPE,
                                        pDeviceDesc->iManufacturer,
                                        (PUCHAR)&StringDescriptorManufacturer,
                                        sizeof(StringDescriptorManufacturer),
                                        &ulBytesTransferred
                                        );
            if (FT_FAILED(ftResult))
            {
                break;
            }

            ftResult = FT_GetDescriptor(a_FTHandle,
                                        FT_STRING_DESCRIPTOR_TYPE,
                                        pDeviceDesc->iProduct,
                                        (PUCHAR)&StringDescriptorProduct,
                                        sizeof(StringDescriptorProduct),
                                        &ulBytesTransferred
                                        );
            if (FT_FAILED(ftResult))
            {
                break;
            }

            ftResult = FT_GetDescriptor(a_FTHandle,
                                        FT_STRING_DESCRIPTOR_TYPE,
                                        pDeviceDesc->iSerialNumber,
                                        (PUCHAR)&StringSerialNumber,
                                        sizeof(StringSerialNumber),
                                        &ulBytesTransferred
                                        );
            if (FT_FAILED(ftResult))
            {
                break;
            }

            CMD_LOG("\n");
            CMD_LOG("\tDEVICE DESCRIPTOR\n");
            CMD_LOG("\tbLength:             0x%02X (%d)\n"     , pDeviceDesc->bLength, pDeviceDesc->bLength);
            CMD_LOG("\tbDescriptorType:     0x%02X\n"          , pDeviceDesc->bDescriptorType);
            CMD_LOG("\tbcdUSB:              0x%04X\n"          , pDeviceDesc->bcdUSB);
            CMD_LOG("\tbDeviceClass:        0x%02X\n"          , pDeviceDesc->bDeviceClass);
            CMD_LOG("\tbDeviceSubClass:     0x%02X\n"          , pDeviceDesc->bDeviceSubClass);
            CMD_LOG("\tbDeviceProtocol:     0x%02X\n"          , pDeviceDesc->bDeviceProtocol);
            CMD_LOG("\tbMaxPacketSize0:     0x%02X (%d)\n"     , pDeviceDesc->bMaxPacketSize0, pDeviceDesc->bMaxPacketSize0);
            CMD_LOG("\tidVendor:            0x%04X\n"          , pDeviceDesc->idVendor);
            CMD_LOG("\tidProduct:           0x%04X\n"          , pDeviceDesc->idProduct);
            CMD_LOG("\tbcdDevice:           0x%04X\n"          , pDeviceDesc->bcdDevice);
            CMD_LOG("\tiManufacturer:       0x%02X (%s)\n"     , pDeviceDesc->iManufacturer, (char *)StringDescriptorManufacturer.szString);
            CMD_LOG("\tiProduct:            0x%02X (%s)\n"     , pDeviceDesc->iProduct, (char *)StringDescriptorProduct.szString);
            CMD_LOG("\tiSerialNumber:       0x%02X (%s)\n"     , pDeviceDesc->iSerialNumber, (char *)StringSerialNumber.szString);
            CMD_LOG("\tbNumConfigurations:  0x%02X\n\n"        , pDeviceDesc->bNumConfigurations);
            break;
        }

        case FT_CONFIGURATION_DESCRIPTOR_TYPE:
        {
            PFT_CONFIGURATION_DESCRIPTOR pConfigDesc = (PFT_CONFIGURATION_DESCRIPTOR)pDescriptor;
            FT_STRING_DESCRIPTOR StringDescriptorConfiguration = {0};
            if (pConfigDesc->iConfiguration)
            {
                ftResult = FT_GetDescriptor(a_FTHandle,
                                            FT_STRING_DESCRIPTOR_TYPE,
                                            pConfigDesc->iConfiguration,
                                            (PUCHAR)&StringDescriptorConfiguration,
                                            sizeof(StringDescriptorConfiguration),
                                            &ulBytesTransferred
                                            );
                if (FT_FAILED(ftResult))
                {
                    break;
                }
            }

            CMD_LOG("\n");
            CMD_LOG("\tCONFIGURATION DESCRIPTOR\n");
            CMD_LOG("\tbLength:             0x%02X (%d)\n"     , pConfigDesc->bLength, pConfigDesc->bLength);
            CMD_LOG("\tbDescriptorType:     0x%02X\n"          , pConfigDesc->bDescriptorType);
            CMD_LOG("\twTotalLength:        0x%04X (%d)\n"     , pConfigDesc->wTotalLength, pConfigDesc->wTotalLength);
            CMD_LOG("\tbNumInterfaces:      0x%02X\n"          , pConfigDesc->bNumInterfaces);
            CMD_LOG("\tbConfigurationValue: 0x%02X\n"          , pConfigDesc->bConfigurationValue);
            CMD_LOG("\tiConfiguration:      0x%02X (%s)\n"     , pConfigDesc->iConfiguration,
                                                                     pConfigDesc->iConfiguration ? (char *)StringDescriptorConfiguration.szString : "NULL");
            CMD_LOG("\tbmAttributes:        0x%02X (%s)\n"     , pConfigDesc->bmAttributes,
                                                                     FT_IS_SELF_POWERED(pConfigDesc->bmAttributes) ? "Self Powered" : "Bus Powered" );
            CMD_LOG("\tMaxPower:            0x%02X (%d mA)\n\n", pConfigDesc->MaxPower, pConfigDesc->MaxPower);
            break;
        }

        case FT_STRING_DESCRIPTOR_TYPE:
        {
            PFT_STRING_DESCRIPTOR pStringDesc = (PFT_STRING_DESCRIPTOR)pDescriptor;

            CMD_LOG("\n");
            CMD_LOG("\tSTRING DESCRIPTOR\n");
            CMD_LOG("\tbLength:             0x%02X (%d)\n"   , pStringDesc->bLength, pStringDesc->bLength);
            CMD_LOG("\tbDescriptorType:     0x%02X\n"        , pStringDesc->bDescriptorType);
            CMD_LOG("\tszString:            %s\n\n"          , (char *)pStringDesc->szString);
            break;
        }

        case FT_INTERFACE_DESCRIPTOR_TYPE:
        {
            PFT_INTERFACE_DESCRIPTOR pIFDesc = (PFT_INTERFACE_DESCRIPTOR)pDescriptor;
            FT_STRING_DESCRIPTOR StringDescriptorInterface = {0};

            if (pIFDesc->iInterface)
            {
                ftResult = FT_GetDescriptor(a_FTHandle,
                                            FT_STRING_DESCRIPTOR_TYPE,
                                            pIFDesc->iInterface,
                                            (PUCHAR)&StringDescriptorInterface,
                                            sizeof(StringDescriptorInterface),
                                            &ulBytesTransferred
                                            );
                if (FT_FAILED(ftResult))
                {
                    break;
                }
            }

            CMD_LOG("\n");
            CMD_LOG("\tINTERFACE DESCRIPTOR\n");
            CMD_LOG("\tbLength:             0x%02X (%d)\n"     , pIFDesc->bLength, pIFDesc->bLength);
            CMD_LOG("\tDescriptorType:      0x%02X\n"          , pIFDesc->bDescriptorType);
            CMD_LOG("\tInterfaceNumber:     0x%02X\n"          , pIFDesc->bInterfaceNumber);
            CMD_LOG("\tbAlternateSetting:   0x%02X\n"          , pIFDesc->bAlternateSetting);
            CMD_LOG("\tbNumEndpoints:       0x%02X\n"          , pIFDesc->bNumEndpoints);
            CMD_LOG("\tbInterfaceClass:     0x%02X\n"          , pIFDesc->bInterfaceClass);
            CMD_LOG("\tbInterfaceSubClass:  0x%02X\n"          , pIFDesc->bInterfaceSubClass);
            CMD_LOG("\tbInterfaceProtocol:  0x%02X\n"          , pIFDesc->bInterfaceProtocol);
            CMD_LOG("\tiInterface:          0x%02X (%s)\n\n"   , pIFDesc->iInterface,
                                                                     pIFDesc->iInterface ? (char *)StringDescriptorInterface.szString : "NULL");
            break;
        }

        default:
        {
            CMD_LOG("\n");
            CMD_LOG("UNKNOWN DESCRIPTOR!!!\n");
            break;
        }
    }
}
