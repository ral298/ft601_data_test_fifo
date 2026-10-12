/*
 * FT600 API Usage Demo App
 *
 * This file has been converted from Windows-specific code to run on a Linux system.
 * Key changes include replacing Windows synchronization primitives with POSIX
 * threading (`pthreads`) functions.
 */

/*
 * Test D3XX interrupt/notification mechanism.
 *
 * Run this with the 'Loopback' image on the FPGA, or
 * at least something that will return bytes in response to
 * FT_WritePipe.
 *
 * Build on Windows using Visual Studio command environment:
 *     cl notify-test.c FTD3XX.lib
 * (Assumes FTD3XX.lib, .dll and .h are in current directory.)
 *
 * Build on Linux:
 *     gcc FTD3xx_Notification.c -L. -lftd3xx -lpthread
 * (Assumes libftd3xx.so and ftd3xx.h is in current directory.)
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include "ftd3xx.h"
#include "event_handle_async.h"
#define EVENT_SIGNATURE 0x45564E54 // ASCII hex for EVNT sequence
#include <unistd.h> // for usleep
#include <errno.h> // ETIMEDOUT
#include <sys/time.h> // gettimeofday
#include <pthread.h>
#include "FTD3XX_DemoApp.h"
#define HOST_TO_DEVICE 0x00
#define DEVICE_TO_HOST 0x80
#define BYTES_WRITE 64
#define SIGNAL_TIMEOUT 100000
// Globals
static char         message[BYTES_WRITE] = "Future Technology Devices International Limited (FTDI-CHIP)";   
static UCHAR        receiveBuffer[BYTES_WRITE];
static ULONG        totalBytesRead = 0;
static ULONG        bytesWritten = 0;
FT_HANDLE           gftHandle;
/*Functions */
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


/**
 * Structure to be used as 'context' for notification callback. 
 *
 * We use this structure to pass information about available data
 * (quantity and endpoint) back to our main thread.
 */
typedef struct MyNotification
{
    FT_HANDLE NotHandle;
    /** An event for the callback to signal to indicate to the
     *  waiting thread that data are available.
     */
    HANDLE  event;
    /** The endpoint with data waiting to be read. */
    UCHAR   endpoint;
    /** The number of bytes waiting to be read. */
    ULONG   bytesToRead;
}
MyNotification;
/**
 * Adjust chip configuration to enable/disable channel notifications.
 *
 * When the chip is reconfigured, it re-boots and the OS re-enumerates
 * it, invalidating the passed-in handle.  So this function closes and
 * re-opens the handle.
 *   xxxx0001 = enable channel 1 and disable channels 2, 3 and 4.
 *   xxxx1100 = enable channels 3 and 4; disable channels 1 and 2.
 *
 * @return FT_OK if successful.
 */
static FT_STATUS enableNotifications()
{
    FT_STATUS                    ftStatus;
    FT_HANDLE                    ftHandle;
    FT_60XCONFIGURATION          chipConfig;
    CMD_LOG("%s:%d: ................Start\n",__FUNCTION__,__LINE__);
    ftStatus = FT_Create((PVOID)"FTDI SuperSpeed-FIFO Bridge", FT_OPEN_BY_DESCRIPTION, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("%s:%d: ERROR: FT_Create failed (%s)\n",__FILE__,__LINE__,statusString(ftStatus));
        return ftStatus;
    }
    memset(&chipConfig, 0, sizeof(chipConfig));

    ftStatus = FT_GetChipConfiguration(ftHandle, &chipConfig);
    if (FT_FAILED(ftStatus))
    {
        FT_Close(ftHandle);
        CMD_LOG("%s:%d: ERROR: FT_GetChipConfiguration failed (%s)\n",__FILE__,__LINE__,statusString(ftStatus));
        return ftStatus;
    }
    /*Enabling all channel Notifications*/
    chipConfig.OptionalFeatureSupport |= (USHORT)CONFIGURATION_OPTIONAL_FEATURE_ENABLENOTIFICATIONMESSAGE_INCHALL;

    ftStatus = FT_SetChipConfiguration(ftHandle, &chipConfig);
    if (FT_FAILED(ftStatus))
    {
        FT_Close(ftHandle);
        CMD_LOG("%s:%d: ERROR: FT_SetChipConfiguration failed (%s)\n",__FILE__,__LINE__, statusString(ftStatus));
        //sleep(1);
        return ftStatus;
    }

    FT_Close(ftHandle);

    sleep(1);
    CMD_LOG("%s:%d: ................END\n",__FUNCTION__,__LINE__);
    return FT_OK;
}

static FT_STATUS DisableNotifications()
{
    FT_HANDLE                    ftHandle;
    FT_STATUS                    ftStatus;
    FT_60XCONFIGURATION          chipConfig;
    CMD_LOG("%s:%d: ................Start\n",__FUNCTION__,__LINE__);
    ftStatus = FT_Create((PVOID)"FTDI SuperSpeed-FIFO Bridge", FT_OPEN_BY_DESCRIPTION, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("%s:%d: ERROR: FT_Create failed (%s)\n",__FILE__,__LINE__,statusString(ftStatus));
        return ftStatus;
    }
    memset(&chipConfig, 0, sizeof(chipConfig));

    ftStatus = FT_GetChipConfiguration(ftHandle, &chipConfig);
    if (FT_FAILED(ftStatus))
    {
        FT_Close(ftHandle);
        CMD_LOG("%s:%d: ERROR: FT_GetChipConfiguration failed (%s)\n",__FILE__,__LINE__,statusString(ftStatus));
        return ftStatus;
    }
    /*Enabling all channel Notifications*/
    chipConfig.OptionalFeatureSupport &= ~(USHORT)CONFIGURATION_OPTIONAL_FEATURE_ENABLENOTIFICATIONMESSAGE_INCHALL;

    ftStatus = FT_SetChipConfiguration(ftHandle, &chipConfig);
    if (FT_FAILED(ftStatus))
    {
        FT_Close(ftHandle);
        CMD_LOG("%s:%d: FT_SetChipConfiguration failed (%s)\n",__FILE__,__LINE__, statusString(ftStatus));
        return ftStatus;
    }

    FT_Close(ftHandle);
    ftHandle = NULL;
    CMD_LOG("%s:%d: ................END ftStatus=%d \n",__FUNCTION__,__LINE__,ftStatus);
    sleep(1);
  
    return FT_OK;
}


/**
 * Invoked by D3XX (on one of its threads) to notify us that
 * there is unread data on a particular endpoint, or that a
 * GPIO event has occurred.
 *
 * @param cbContext  address of a structure owned by our app.
 * @param cbType     subject of notification: unread data or GPIO.
 * @param cbInfo     information about the unread data (or GPIO event).
 */

static VOID notificationCb(void *cbContext, E_FT_NOTIFICATION_CALLBACK_TYPE cbType, void *cbInfo)
{
    MyNotification *notification = cbContext;
    FT_STATUS       ftStatus = FT_OK;
    CMD_LOG("%s:%d: ................Start\n",__FUNCTION__,__LINE__);
    if (!notification || !notification->event)
    {
        CMD_LOG("%s:%d: Invalid callback context or event.\n", __FILE__, __LINE__);
        return;
    }
    if (cbType == E_FT_NOTIFICATION_CALLBACK_TYPE_DATA)
    {
        // There is unread data at one of the endpoints.
        FT_NOTIFICATION_CALLBACK_INFO_DATA *info = cbInfo;
        CMD_LOG( "%s:%d: [D3XX thread] In callback!  %d bytes available on endpoint 0x%02X\n",__FILE__,__LINE__,
            (int)info->ulRecvNotificationLength,(unsigned int)info->ucEndpointNo);
        
        notification->endpoint = (unsigned int)info->ucEndpointNo;
        notification->bytesToRead = (int)info->ulRecvNotificationLength;


        // Tell app thread to read data.  FIXME: need queue here?
        (void)FT_W32_SetEvent(notification->event);


        if ((notification->endpoint & DEVICE_TO_HOST) != DEVICE_TO_HOST)
        {
            CMD_LOG("%s:%d: ERROR: Unexpected notification for endpoint %d.\n",__FILE__,__LINE__,notification->endpoint);
            ftStatus = FT_IO_ERROR;
            return;
        }

	while (totalBytesRead < bytesWritten)
	{
	    ULONG currentBytesRead = 0; // Use a temporary variable for the current read
	    // Read up to `bytesToRead` into the buffer, offsetting by the total bytes read so far.
	    ftStatus = FT_ReadPipe(notification->NotHandle, notification->endpoint, receiveBuffer + totalBytesRead,
	                            notification->bytesToRead, &currentBytesRead, 0);

	    if (FT_FAILED(ftStatus))
	    {
	        CMD_LOG("%s:%d: ERROR: FT_ReadPipe failed %d (%s)\n", __FILE__, __LINE__, ftStatus, statusString(ftStatus));
	        return;
	    }
    
	    CMD_LOG("%s:%d: Read %u bytes. receiveBuffer = %s\n", __FILE__, __LINE__, (unsigned int)currentBytesRead,receiveBuffer);
	    totalBytesRead += currentBytesRead;
	}

    }
    else
    {
        // Only callback types are _DATA 
        assert(cbType == E_FT_NOTIFICATION_CALLBACK_TYPE_GPIO);
    }
    CMD_LOG("%s:%d: ................END\n",__FUNCTION__,__LINE__);
}


/**
 * Test FT_SetNotificationCallback by writing data to an endpoint
 * and waiting for notification that the data has looped back.
 *
 * @param endpoint  the endpoint to test.
 *
 * @return FT_OK if successful.
 */
static FT_STATUS loopbackNotify(UCHAR endpoint)
{
    FT_STATUS ftStatus = FT_OK;

    assert(endpoint >= 2 && endpoint <= 5);

    // ✅ Allocate MyNotification on heap to survive async callbacks
    MyNotification *notification = calloc(1, sizeof(MyNotification));
    if (!notification)
    {
        FT_Close(gftHandle);
        CMD_LOG("%s:%d: ERROR: Failed to allocate notification structure.\n", __FILE__, __LINE__);
        return FT_INSUFFICIENT_RESOURCES;
    }

    notification->event = FT_W32_CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!notification->event)
    {
        CMD_LOG("%s:%d: ERROR: Failed to create event.\n", __FILE__, __LINE__);
        ftStatus = FT_NO_SYSTEM_RESOURCES;
        goto cleanup;
    }

    if (((Event *)notification->event)->signature != EVENT_SIGNATURE)
    {
        CMD_LOG("%s:%d: ERROR: Invalid event signature.\n", __FILE__, __LINE__);
        ftStatus = FT_NO_SYSTEM_RESOURCES;
        goto cleanup;
    }

    notification->NotHandle = gftHandle;

    ftStatus = FT_SetNotificationCallback(gftHandle, notificationCb, notification);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("%s:%d: ERROR: FT_SetNotificationCallback failed (%s)\n", __FILE__, __LINE__, statusString(ftStatus));
        goto cleanup;
    }

    CMD_LOG("%s:%d: Writing %u bytes to endpoint 0x%02X.\n", __FILE__, __LINE__,
           BYTES_WRITE, (unsigned int)(endpoint | HOST_TO_DEVICE));

    ftStatus = FT_WritePipe(gftHandle, endpoint | HOST_TO_DEVICE,
                            (UCHAR *)message, BYTES_WRITE, &bytesWritten, 0);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("%s:%d: ERROR: FT_WritePipe failed (%s)\n", __FILE__, __LINE__, statusString(ftStatus));
        goto cleanup;
    }

    DWORD waitResult = FT_W32_WaitForSingleObject(notification->event, SIGNAL_TIMEOUT);
    if (waitResult == WAIT_FAILED)
    {
        CMD_LOG("%s:%d: ERROR: Wait failed.\n", __FILE__, __LINE__);
        ftStatus = FT_NO_SYSTEM_RESOURCES;
        goto cleanup;
    }
    else if (waitResult == WAIT_TIMEOUT)
    {
        CMD_LOG("%s:%d: ERROR: Wait timed out.\n", __FILE__, __LINE__);
        ftStatus = FT_TIMEOUT;
        goto cleanup;
    }

    assert(waitResult == WAIT_OBJECT_0);
    CMD_LOG("%s:%d: Wrote %u bytes, write buffer = %s.\n", __FILE__, __LINE__, bytesWritten, message);

cleanup:
    FT_ClearNotificationCallback(gftHandle);

    if (notification)
    {
        if (notification->event)
            FT_W32_CloseHandle(notification->event);

        free(notification);
    }
    FT_Close(gftHandle);
    sleep(1);
    ftStatus = DisableNotifications();
    CMD_LOG("%s:%d: ................END\n", __FUNCTION__, __LINE__);
    return ftStatus;
}

// --- Function Implementations ---

///////////////////////////////////////////////////////////////////////////////////
// Demonstrates reading from IN pipes using notification messaging
// FT_Create              Open a device
// FT_SetNotificationCallback Register the callback function for DATA availability
// FT_ClearNotificationCallback Unregister the callback function
// FT_Close               Close device handle
///////////////////////////////////////////////////////////////////////////////////
BOOL NotificationDataTest()
{
    FT_STATUS   ftStatus = FT_OK;

    CMD_LOG("%s:%d: Attempting to open FT60X device...\n",__FILE__,__LINE__);
    // Enable notifications of unread data (for all channels).
    ftStatus = enableNotifications();
    if (FT_FAILED(ftStatus))
    {
        return (ftStatus == FT_OK);
    }
    ftStatus = FT_Create((PVOID)"FTDI SuperSpeed-FIFO Bridge", FT_OPEN_BY_DESCRIPTION, &gftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("%s:%d: ERROR: FT_Create failed (%s)\n",__FILE__,__LINE__,statusString(ftStatus));
        return (ftStatus == FT_OK);
    }
    ftStatus = loopbackNotify((UCHAR)2);
    if (FT_FAILED(ftStatus))
    {
        return (ftStatus == FT_OK);
    }
    CMD_LOG("%s:%d: ................END ftStatus=%d \n",__FUNCTION__,__LINE__,ftStatus);
    return (ftStatus == FT_OK);

}
