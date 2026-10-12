/*
* FT600 API Usage Demo App
*
* Copyright (C) 2015 FTDI Chip
* FTD3XX_GPIO.c
*
* Converted to Linux C code.
*/

// Standard C headers for Linux
#include <stdio.h>
#include <stdbool.h>

// Assuming ftd3xx.h is available in the include path
#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"

// Define CMD_LOG for printing, as it's not defined in the original snippet.
// This is a standard macro to print to the console.
#define CMD_LOG(...) printf(__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////
// Demonstrates configuring the timeout values for read endpoints.
// FT_Create                  Open a device
// FT_EnableGPIO              Configures GPIO.
// FT_ReadGPIO                Gets GPIO pin status
// FT_WriteGPIO               Sets GPIO pins to low or high.
// FT_Close                   Close device handle
///////////////////////////////////////////////////////////////////////////////////
BOOL GPIOTest()
{
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle;
    // Replace Windows-specific BOOL with standard C bool
    bool bResult = true;
    uint32_t u32Data = 0;

    //
    // Open a device
    //
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        bResult = false;
        // In Linux, it's more common to return 0 for success and a non-zero value for failure,
        // but here we are returning a bool, which is fine.
        return false;
    }

    // Get GPIO status
    ftStatus = FT_ReadGPIO(ftHandle, &u32Data);
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_ReadGPIO failed\n");
        bResult = false;
        goto exit;
    }

    // Removed _T macro
    CMD_LOG("\t Initial GPIO bitmap : %d\n", u32Data);

    // Removed _T macro
    CMD_LOG("\t moving both the GPIOs to Output mode\n");
    ftStatus = FT_EnableGPIO(ftHandle, 0x3, 0x3); //bit 0 and 1 both set.
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_EnableGPIO failed\n");
        bResult = false;
        goto exit;
    }

    // Get GPIO status
    ftStatus = FT_ReadGPIO(ftHandle, &u32Data);
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_ReadGPIO failed\n");
        bResult = false;
        goto exit;
    }
    // Removed _T macro
    CMD_LOG("\t GPIO bitmap after EnableGPIO : %d\n", u32Data);
#if 0
    ftStatus = FT_SetGPIOPull(ftHandle, 0x3, 0x3);
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_SetGPIOPull failed\n");
        bResult = FALSE;
        goto exit;
    }
#endif
    // Removed _T macro
    CMD_LOG("\t Making both the GPIO high\n");
    // set both the GPIOs to high.
    ftStatus = FT_WriteGPIO(ftHandle, 0x3, 0x3);
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_WriteGPIO failed\n");
        bResult = false;
        goto exit;
    }

    // Get GPIO status
    ftStatus = FT_ReadGPIO(ftHandle, &u32Data);
    if (FT_FAILED(ftStatus))
    {
        // Removed _T macro
        CMD_LOG("\t FT_ReadGPIO failed\n");
        bResult = false;
        goto exit;
    }
    // Removed _T macro
    CMD_LOG("\t GPIO bitmap after FT_WriteGPIO : %d\n", u32Data);

exit:
    //
    // Lets restore the original GPIO states. 
    //
    ftStatus = FT_EnableGPIO(ftHandle, 0x3, 0x3);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("\t%s FAILED! FT_EnableGPIO\n", __FUNCTION__);
        return false;
    }

    /* Set the pins to LOW */
    ftStatus = FT_WriteGPIO(ftHandle,
        0x3, /* mask - GPIO_0 and GPIO_1 are being written*/
        0); /* value for GPIO_0 and GPIO_1 */
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("\t%s FAILED! FT_WriteGPIO\n", __FUNCTION__);
        return false;
    }


    //
    // Close device handle
    //
    FT_Close(ftHandle);
    ftHandle = NULL;


    return bResult;
}
