#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"

#define FIFO_CHANNEL_1	0
#define FIFO_CHANNEL_2	1
#define FIFO_CHANNEL_3	2
#define FIFO_CHANNEL_4	3
#define TIMEOUT			5000//0xFFFFFFFF

BOOL AbortRecovery(void)
{
	FT_HANDLE handle = NULL;
	FT_STATUS ftStatus = FT_OK;
	uint32_t to_read = 4096;

	ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &handle);

	if (!handle) {
		CMD_LOG("Failed to create device ftStatus=%d\r\n",ftStatus);
		return -1;
	}
	CMD_LOG("Device created\r\n");

	uint8_t *in_buf = (uint8_t *)malloc(to_read);
	DWORD count;

	while(1)
	{
        if (FT_OK != FT_ReadPipeEx(handle, FIFO_CHANNEL_1, in_buf, to_read,
            &count, TIMEOUT)) {
            CMD_LOG("Read timeout ,FIFO is empty\r\n");
            goto _Exit;
        }
        CMD_LOG("Read %d bytes\r\n", count);
		break;
	}

_Exit:
	free(in_buf);
	UCHAR ucDirection = (FT_GPIO_DIRECTION_OUT << FT_GPIO_0) ;
	UCHAR ucMask = (FT_GPIO_VALUE_HIGH << FT_GPIO_0);
	ftStatus = FT_EnableGPIO(handle, ucMask, ucDirection);
    if(ftStatus != FT_OK)
	{
		CMD_LOG("FT_EnableGPIO is Failed...=%d\n",ftStatus);
		return 0;
	}
	ftStatus = FT_WriteGPIO(handle,ucMask, (FT_GPIO_VALUE_HIGH << FT_GPIO_0));
	    if(ftStatus != FT_OK)
	{
		CMD_LOG("FT_WriteGPIO is Failed...=%d\n",ftStatus);
		return 0;
	}
	ftStatus = FT_AbortPipe(handle,0x82);	
	ftStatus = FT_WriteGPIO(handle,ucMask, (FT_GPIO_VALUE_LOW << FT_GPIO_0));

    FT_Close(handle);
	return (ftStatus == FT_OK);
}

