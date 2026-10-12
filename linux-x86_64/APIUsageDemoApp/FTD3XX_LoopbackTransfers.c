/*
 * FT600 API Usage Demo App
 *
 * Copyright (C) 2015 FTDI Chip
 *
 * This file demonstrates different types of loopback testing
 * using synchronous transfers and asynchronous transfers
 * as well as the streaming feature.
 * file: FTD3XX_LoopbackTransfers.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // POSIX threads header
#include <unistd.h>
#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"
// A note on the FTDI library: This code assumes the Linux FTDI library
// provides similar function names and behavior to its Windows counterpart.
// Overlapped I/O is a Windows concept. The equivalent in Linux would typically
// involve non-blocking I/O with select, poll, epoll, or aio.
// Since the original code polls for FT_IO_PENDING, we'll keep that polling logic
// as a direct conversion, although a more robust Linux application might use a
// different approach.
#define BUFFER_SIZE                 4096

#define MULTI_ASYNC_BUFFER_SIZE    4096 //1048576 //32768  // 8388608 //   
#define MULTI_ASYNC_NUM             4         
#define NUM_ITERATIONS              1

pthread_t   writeThread;
pthread_t   readThread;
int         writerRunning = 0;
int         readerRunning = 0;
int         r = 0;
int         exitWriter = 0;
int         exitReader = 0;
UCHAR *acWriteBuf = NULL;
UCHAR *acReadBuf = NULL;
ULONG ulBytesToWrite = MULTI_ASYNC_BUFFER_SIZE;
ULONG ulBytesToRead = MULTI_ASYNC_BUFFER_SIZE;
ULONG ulBytesWritten[MULTI_ASYNC_NUM] = {0};
ULONG ulBytesRead[MULTI_ASYNC_NUM] = {0};
OVERLAPPED vOverlappedWrite[MULTI_ASYNC_NUM] = {{0}};
OVERLAPPED vOverlappedRead[MULTI_ASYNC_NUM] = {{0}};
FT_HANDLE ftHandle;
//
// Demonstrates single channel data transfer loopback function using synchronous write and read
//
BOOL LoopbackTest() {
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle;
    bool bResult = true;

    // Open a device
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus)) {
        return false;
    }

    ftStatus = FT_EnableGPIO(ftHandle, 0x3, 0x3);
    if (FT_FAILED(ftStatus)) {
        fprintf(stderr, "\t FT_EnableGPIO failed\n");
        bResult = false;
        goto exit;
    }

    // Drive the GPIO LOW for data transfer.
    // otherwise FPGA will drop the data
    ftStatus = FT_WriteGPIO(ftHandle, 0x3, 0);
    if (FT_FAILED(ftStatus)) {
        fprintf(stderr, "\t FT_WriteGPIO failed\n");
        bResult = false;
        goto exit;
    }

    // Write and read loopback transfer
    uint32_t dwNumIterations = NUM_ITERATIONS;
    for (uint32_t i = 0; i < dwNumIterations; i++) {
        // Write to channel 1 ep 0x02
        unsigned char acWriteBuf[BUFFER_SIZE] = {0xFF};
        unsigned int ulBytesWritten = 0;
        CMD_LOG("\tRequesting to write %zu bytes...\n", sizeof(acWriteBuf));
        ftStatus = FT_WritePipe(ftHandle, 0x02, acWriteBuf, sizeof(acWriteBuf), &ulBytesWritten, 5000);
        CMD_LOG("\tActually wrote %u bytes.\n", ulBytesWritten);        
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }
        if (ulBytesWritten != sizeof(acWriteBuf)) {
            bResult = false;
            goto exit;
        }

        // Read from channel 1 ep 0x82
        // FT_ReadPipe is a blocking/synchronous function.
        // It will not return until it has received all data requested
        unsigned char acReadBuf[BUFFER_SIZE] = {0xAA};
        unsigned int ulBytesRead = 0;
        CMD_LOG("\tRequesting up to %zu bytes...\n", sizeof(acReadBuf));
        ftStatus = FT_ReadPipe(ftHandle, 0x82, acReadBuf, sizeof(acReadBuf), &ulBytesRead, 5000);
        CMD_LOG("\tRead %u bytes.\n", ulBytesRead);        
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }
        if (ulBytesRead != sizeof(acReadBuf)) {
            bResult = false;
            goto exit;
        }

        // Verify if data written is same as data read
        if (memcmp(acWriteBuf, acReadBuf, ulBytesRead)) {
            bResult = false;
            goto exit;
        }
    }

exit:
    // Close device
    FT_Close(ftHandle);
    return bResult;
}

//
// Demonstrates single channel loopback function using asynchronous write and read
//
BOOL AsyncLoopbackTest() {
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle;
    bool bResult = true;

    // Open a device
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus)) {
        return false;
    }

    // Write and read loopback transfer
    uint32_t dwNumIterations = NUM_ITERATIONS;
    for (uint32_t i = 0; i < dwNumIterations; i++) {
        // Write to channel 1 ep 0x02
        unsigned char acWriteBuf[BUFFER_SIZE] = {0xFF};
        unsigned int ulBytesWritten = 0;
        unsigned int ulBytesToWrite = sizeof(acWriteBuf);

        // This part needs a significant refactor on Linux, as there's no direct "Overlapped" equivalent.
        // We will simulate the behavior with a non-blocking poll loop.
        // The FTDI Linux library may handle this differently.
        OVERLAPPED vOverlappedWrite = {0};
        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedWrite);
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }

        CMD_LOG("\tRequesting to write %u bytes...\n", ulBytesToWrite);
        ftStatus = FT_WritePipeAsync(ftHandle, 0, acWriteBuf, ulBytesToWrite, &ulBytesWritten, &vOverlappedWrite);
        if (ftStatus == FT_IO_PENDING) {
            do {
                ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedWrite, &ulBytesWritten, false);
                if (ftStatus == FT_IO_INCOMPLETE) {
                    continue;
                } else if (FT_FAILED(ftStatus)) {
                    FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite);
                    bResult = false;
                    goto exit;
                } else {
                    CMD_LOG("\tActually wrote %u bytes.\n", ulBytesWritten);
                    break;
                }
            } while (1);
        } else if (FT_FAILED(ftStatus)) {
            FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite);
            bResult = false;
            goto exit;
        }

        if (ulBytesWritten != ulBytesToWrite) {
            FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite);
            bResult = false;
            goto exit;
        }

        FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite);

        // Read from channel 1 ep 0x82
        unsigned char acReadBuf[BUFFER_SIZE] = {0xAA};
        unsigned int ulBytesRead = 0;
        unsigned int ulBytesToRead = sizeof(acReadBuf);

        OVERLAPPED vOverlappedRead = {0};
        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedRead);
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }

        CMD_LOG("\tRequesting to read %u bytes...\n", ulBytesToRead);
        ftStatus = FT_ReadPipeAsync(ftHandle, 0, acReadBuf, ulBytesToRead, &ulBytesRead, &vOverlappedRead);
        if (ftStatus == FT_IO_PENDING) {
            do {
                ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedRead, &ulBytesRead, false);
                if (ftStatus == FT_IO_INCOMPLETE) {
                    continue;
                } else if (FT_FAILED(ftStatus)) {
                    FT_ReleaseOverlapped(ftHandle, &vOverlappedRead);
                    bResult = false;
                    goto exit;
                } else {
                    CMD_LOG("\tActually read %u bytes.\n", ulBytesRead);
                    break;
                }
            } while (1);
        } else if (FT_FAILED(ftStatus)) {
            FT_ReleaseOverlapped(ftHandle, &vOverlappedRead);
            bResult = false;
            goto exit;
        }

        if (ulBytesRead != ulBytesToRead) {
            FT_ReleaseOverlapped(ftHandle, &vOverlappedRead);
            bResult = false;
            goto exit;
        }

        FT_ReleaseOverlapped(ftHandle, &vOverlappedRead);

        if (memcmp(acWriteBuf, acReadBuf, ulBytesRead)) {
            bResult = false;
            goto exit;
        }
    }

exit:
    FT_Close(ftHandle);
    return bResult;
}

//
// Demonstrates single channel loopback function using multiple asynchronous write and read
//
BOOL MultiAsyncLoopbackTest() {
    FT_STATUS ftStatus = FT_OK;
    FT_HANDLE ftHandle;
    unsigned char* acWriteBuf = (unsigned char*)malloc(MULTI_ASYNC_NUM * MULTI_ASYNC_BUFFER_SIZE);
    unsigned char* acReadBuf = (unsigned char*)malloc(MULTI_ASYNC_NUM * MULTI_ASYNC_BUFFER_SIZE);
    unsigned int ulBytesWritten[MULTI_ASYNC_NUM] = {0};
    unsigned int ulBytesRead[MULTI_ASYNC_NUM] = {0};
    OVERLAPPED vOverlappedWrite[MULTI_ASYNC_NUM] = {0};
    OVERLAPPED vOverlappedRead[MULTI_ASYNC_NUM] = {0};
    unsigned int ulBytesToWrite = MULTI_ASYNC_BUFFER_SIZE;
    unsigned int ulBytesToRead = MULTI_ASYNC_BUFFER_SIZE;
    bool bResult = true;

    if (!acWriteBuf || !acReadBuf) {
        bResult = false;
        goto exit;
    }

    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (FT_FAILED(ftStatus)) {
        goto exit;
    }

    ftStatus = FT_SetStreamPipe(ftHandle, false, false, 0x02, ulBytesToWrite);
    if (FT_FAILED(ftStatus)) {
        bResult = false;
        goto exit;
    }
    ftStatus = FT_SetStreamPipe(ftHandle, false, false, 0x82, ulBytesToRead);
    if (FT_FAILED(ftStatus)) {
        bResult = false;
        goto exit;
    }

    for (uint32_t j = 0; j < MULTI_ASYNC_NUM; j++) {
        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedWrite[j]);
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }

        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedRead[j]);
        if (FT_FAILED(ftStatus)) {
            bResult = false;
            goto exit;
        }
    }

    uint32_t dwNumIterations = NUM_ITERATIONS;
    for (uint32_t i = 0; i < dwNumIterations; i++) {
        for (uint32_t j = 0; j < MULTI_ASYNC_NUM; j++) {
            memset(&acWriteBuf[j * MULTI_ASYNC_BUFFER_SIZE], 0x55 + j, ulBytesToWrite);
            vOverlappedWrite[j].Internal = 0;
            vOverlappedWrite[j].InternalHigh = 0;
            ulBytesWritten[j] = 0;

            CMD_LOG("\tRequesting to write %u bytes...\n", ulBytesToWrite);
            ftStatus = FT_WritePipeAsync(ftHandle, 0, &acWriteBuf[j * MULTI_ASYNC_BUFFER_SIZE], ulBytesToWrite, &ulBytesWritten[j], &vOverlappedWrite[j]);
            if (ftStatus != FT_IO_PENDING) {
                bResult = false;
                goto exit;
            }
        }

        for (uint32_t j = 0; j < MULTI_ASYNC_NUM; j++) {
            memset(&acReadBuf[j * MULTI_ASYNC_BUFFER_SIZE], 0xAA + j, ulBytesToRead);
            vOverlappedRead[j].Internal = 0;
            vOverlappedRead[j].InternalHigh = 0;
            ulBytesRead[j] = 0;

            CMD_LOG("\tRequesting to read %u bytes...\n", ulBytesToRead);
            ftStatus = FT_ReadPipeAsync(ftHandle, 0, &acReadBuf[j * MULTI_ASYNC_BUFFER_SIZE], ulBytesToRead, &ulBytesRead[j], &vOverlappedRead[j]);
            if (ftStatus != FT_IO_PENDING) {
                bResult = false;
                goto exit;
            }
        }

        for (uint32_t j = 0; j < MULTI_ASYNC_NUM; j++) {
            ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedWrite[j], &ulBytesWritten[j], true);
            if (FT_FAILED(ftStatus)) {
                bResult = false;
                goto exit;
            }
            if (ulBytesWritten[j] != ulBytesToWrite) {
                bResult = false;
                goto exit;
            }
            CMD_LOG("\tActually wrote %u bytes.\n", ulBytesWritten[j]);

            ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedRead[j], &ulBytesRead[j], true);
            if (FT_FAILED(ftStatus)) {
                bResult = false;
                goto exit;
            }
            if (ulBytesRead[j] != ulBytesToRead) {
                bResult = false;
                goto exit;
            }
            CMD_LOG("\tActually read %u bytes DONE!\n", ulBytesRead[j]);

            if (memcmp(&acWriteBuf[j * MULTI_ASYNC_BUFFER_SIZE], &acReadBuf[j * MULTI_ASYNC_BUFFER_SIZE], ulBytesToRead)) {
                bResult = false;
                goto exit;
            }
        }
    }

exit:
    for (uint32_t j = 0; j < MULTI_ASYNC_NUM; j++) {
        FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite[j]);
        FT_ReleaseOverlapped(ftHandle, &vOverlappedRead[j]);
    }

    FT_ClearStreamPipe(ftHandle, false, false, 0x02);
    FT_ClearStreamPipe(ftHandle, false, false, 0x82);

    FT_Close(ftHandle);
    free(acWriteBuf);
    free(acReadBuf);
    return bResult;
}

static void* asyncRead(void *arg)
{
    int i,j;
    (void)arg;
    FT_STATUS ftStatus = FT_OK;
    BOOL bResult = TRUE;
    UCHAR *acReadBuf = calloc(MULTI_ASYNC_NUM * MULTI_ASYNC_BUFFER_SIZE, sizeof(UCHAR));
    CMD_LOG("Starting %s.\n", __FUNCTION__);
    /*Create the overlapped io event for asynchronous transfer*/
    for (j=0; j<MULTI_ASYNC_NUM; j++)
    {
        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedRead[j]);
        if (FT_FAILED(ftStatus))
        {
            CMD_LOG("FT_InitializeOverlapped failed!\n");
            bResult = FALSE;
            break;
        }
    }
    while (!exitReader) // global thread-exit flag
    {    
        /* Read loopback transfer*/
        for (i=0; i<NUM_ITERATIONS; i++)
        {
            /*Send asynchronous read transfer requests*/
            for (j=0; j<MULTI_ASYNC_NUM; j++)
            {
                memset(&acReadBuf[j*MULTI_ASYNC_BUFFER_SIZE], 0xAA+j, ulBytesToRead);
                vOverlappedRead[j].Internal = 0;
                vOverlappedRead[j].InternalHigh = 0;
                ulBytesRead[j] = 0;
                
                /*
                Read asynchronously
                FT_ReadPipe is a blocking/synchronous function.
                To make it unblocking/asynchronous operation, vOverlapped parameter is supplied.
                When FT_ReadPipe is called with overlapped io, the function will immediately return with FT_IO_PENDING
                */ 
                ftStatus = FT_ReadPipeAsync( ftHandle, 0, 
                                        &acReadBuf[j*MULTI_ASYNC_BUFFER_SIZE], ulBytesToRead, 
                                        &ulBytesRead[j], &vOverlappedRead[j]);
                if (ftStatus != FT_IO_PENDING)
                {
                    CMD_LOG("FT_ReadPipe failed! Status=%d\n",ftStatus);
                    bResult = FALSE;
                    goto exit;
                }

                ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedRead[j], &ulBytesRead[j], TRUE);
                if (FT_FAILED(ftStatus))
                {
                    bResult = FALSE;
                    goto exit;
                }
                if (ulBytesRead[j] != ulBytesToRead)
                {
                    //CMD_LOG("FT_GetOverlappedResult failed! ulBytesRead[j=%d]=%d != %d ulBytesToRead  \n",j,ulBytesRead[j],ulBytesToRead);
                    goto exit;
                }
                else
                {
                    //CMD_LOG("FT_GetOverlappedResult ulBytesRead[j=%d]=%d == %d ulBytesToRead  \n",j,ulBytesRead[j],ulBytesToRead);
                }

            }
        }
        exit:bResult = FALSE;

        if(bResult == FALSE)
        {
            break;
        }
    }
    
    /*Delete the overlapped io event for asynchronous transfer*/
    for (j=0; j<MULTI_ASYNC_NUM; j++)
    {
        FT_ReleaseOverlapped(ftHandle, &vOverlappedRead[j]);
    }
    free(acReadBuf);
    CMD_LOG("Exiting %s.\n", __FUNCTION__);

    return NULL;
}

static void* asyncWrite(void *arg)
{
    int i,j;
    (void)arg;
    FT_STATUS ftStatus = FT_OK;
    BOOL bResult = TRUE;
    UCHAR *acWriteBuf = calloc(MULTI_ASYNC_NUM * MULTI_ASYNC_BUFFER_SIZE, sizeof(UCHAR));
    CMD_LOG("Starting %s.\n", __FUNCTION__);
    /* Create the overlapped io event for asynchronous transfer*/
    for (j=0; j<MULTI_ASYNC_NUM; j++)
    {
        ftStatus = FT_InitializeOverlapped(ftHandle, &vOverlappedWrite[j]);
        if (FT_FAILED(ftStatus))
        {
            CMD_LOG("FT_InitializeOverlapped failed!\n");
            bResult = FALSE;
            break;
        }
    }

    while (!exitWriter) // global thread-exit flag
    {
        /*Write loopback transfer*/
        for (i=0; i<NUM_ITERATIONS; i++)
        {
            /*Send asynchronous write transfer requests*/
            for (j=0; j<MULTI_ASYNC_NUM; j++)
            {
                memset(&acWriteBuf[j*MULTI_ASYNC_BUFFER_SIZE], 0x55+j, ulBytesToWrite);
                vOverlappedWrite[j].Internal = 0;
                vOverlappedWrite[j].InternalHigh = 0;
                ulBytesWritten[j] = 0;
                
                /* Write asynchronously
                * FT_WritePipe is a blocking/synchronous function.
                * To make it unblocking/asynchronous operation, vOverlapped parameter is supplied.
                * When FT_WritePipe is called with overlapped io, the function will immediately return with FT_IO_PENDING
                */
                ftStatus = FT_WritePipeAsync(ftHandle, 0, 
                                        &acWriteBuf[j*MULTI_ASYNC_BUFFER_SIZE], ulBytesToWrite, 
                                        &ulBytesWritten[j], &vOverlappedWrite[j]);
                if (ftStatus != FT_IO_PENDING)
                {
                    CMD_LOG("FT_WritePipe failed! Status=%d\n",ftStatus);
                    bResult = FALSE;
                    goto exit;
                }
            }

            /*Wait for the asynchronous write and read transfer requests to finish*/
            for (j=0; j<MULTI_ASYNC_NUM; j++)
            {
                ftStatus = FT_GetOverlappedResult(ftHandle, &vOverlappedWrite[j], &ulBytesWritten[j], TRUE);
                if (FT_FAILED(ftStatus))
                {
                    CMD_LOG("FT_GetOverlappedResult failed!->Write\n");
                    bResult = FALSE;
                    ftStatus = FT_AbortPipe(ftHandle, 0x82);
                    CMD_LOG("Write -> FT_AbortPipe return =%d  \n",ftStatus);
                    goto exit;
                }
                if (ulBytesWritten[j] != ulBytesToWrite)
                {
                    //CMD_LOG("FT_GetOverlappedResult >> ulBytesWritten[j=%d]=%d != %d ulBytesToWrite\n",j,ulBytesWritten[j],ulBytesToWrite);
                    bResult = FALSE;
                    goto exit;  
                }
                else
                {
                    //CMD_LOG("FT_GetOverlappedResult >> ulBytesWritten[j=%d]=%d == %d ulBytesToWrite\n",j,ulBytesWritten[j],ulBytesToWrite);

                }
            }
        }
        exit:bResult = FALSE;
        if(bResult == FALSE)
        {
            break;
        }
    }
    /*Delete the overlapped io event for asynchronous transfer*/
    for (j=0; j<MULTI_ASYNC_NUM; j++)
    {
        FT_ReleaseOverlapped(ftHandle, &vOverlappedWrite[j]);
    }
  
    free(acWriteBuf);
    CMD_LOG("Exiting %s.\n", __FUNCTION__);
    return NULL;
}

BOOL AsyncReadWriteLoopbackTest()
{
FT_STATUS ftStatus = FT_OK;
    BOOL bResult = TRUE;

    /*Open device by description*/
    ftStatus = FT_Create((PVOID)"FTDI SuperSpeed-FIFO Bridge",
                   FT_OPEN_BY_DESCRIPTION, &ftHandle);
    if (FT_FAILED(ftStatus))
    {
        CMD_LOG("FT_Create failed!=%d\n",ftStatus);
        goto exit;
    }

    FT_SetPipeTimeout(ftHandle,0x02,5000);
    FT_SetPipeTimeout(ftHandle,0x82,5000);
       // Start thread to write bytes
    r = pthread_create(&writeThread, NULL, &asyncWrite, NULL);
    if (r != 0)
    {
        CMD_LOG("Failed to create write thread (%d)\n", r);
        goto exit;
    }
    writerRunning = 1;

    // Start thread to read bytes
    r = pthread_create(&readThread, NULL, &asyncRead,NULL);
    if (r != 0)
    {
        CMD_LOG("Failed to create read thread (%d)\n", r);
        goto exit;
    }
    readerRunning = 1;

    // Wait for threads to finish
    (void)pthread_join(writeThread, NULL);
    writerRunning = 0;
    (void)pthread_join(readThread, NULL);
    readerRunning = 0;
    
exit:
    if (readerRunning)
    {
        exitReader = 1;
        (void)pthread_join(readThread, NULL);
    }
    if (writerRunning)
    {
        exitWriter = 1;
        (void)pthread_join(writeThread, NULL);
    }

    /*Close device*/
    FT_Close(ftHandle);

    if(acReadBuf != NULL)
        free(acReadBuf);

    if(acWriteBuf != NULL)
        free(acWriteBuf);
    
    return bResult;
}

#if 0
BOOL ReadPipeExWritePipeExLoopbackTest() 
{
	FT_HANDLE handle = NULL;
    unsigned short vendorId;
    unsigned short productId;
	uint32_t to_write = 4096;
	uint32_t to_read = 4096;

	FT_Create(0, FT_OPEN_BY_INDEX, &handle);

	if (!handle) {
		CMD_LOG("Failed to create device\r\n");
		return -1;
	}
	CMD_LOG("Device created\r\n");

    FT_STATUS ftStatus = FT_GetVIDPID(handle,&vendorId,&productId);
    if (ftStatus != FT_OK)
    {
        (void)CMD_LOG("FT_GetVIDPID failed %d\n",ftStatus);
    }
    (void)CMD_LOG("\tVendor ID: 0x%04X\n\tProduct ID: 0x%04X\n",vendorId, productId);

    FT_SetPipeTimeout(handle,0x02,5000);
    FT_SetPipeTimeout(handle,0x82,5000);
    
	uint8_t *out_buf = (uint8_t *)malloc(to_write);
	uint8_t *in_buf = (uint8_t *)malloc(to_read);
	DWORD count;

	if (FT_OK != FT_WritePipeEx(handle, FIFO_CHANNEL_1, out_buf, to_write,
				&count, TIMEOUT)) {
		CMD_LOG("Failed to write\r\n");
		goto _Exit;
	}
	CMD_LOG("Wrote %d bytes\r\n", count);

	if (FT_OK != FT_ReadPipeEx(handle, FIFO_CHANNEL_1, in_buf, to_read,
				&count, TIMEOUT)) {
		CMD_LOG("Failed to read\r\n");
		goto _Exit;
	}
	CMD_LOG("Read %d bytes\r\n", count);

_Exit:
	free(in_buf);
	free(out_buf);
	FT_Close(handle);
	return 0;
}
#endif