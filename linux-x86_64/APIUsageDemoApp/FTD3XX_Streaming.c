#define _POSIX_C_SOURCE 200809L  // Needed for clock_gettime and clock_nanosleep

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#endif

#include "ftd3xx.h"
#include "FTD3XX_DemoApp.h"


static bool do_exit;
static atomic_int tx_count;
static atomic_int rx_count;
static uint8_t in_ch_cnt;
static uint8_t out_ch_cnt;

static pthread_t measure_thread;
static pthread_t write_thread;
static pthread_t read_thread;
static const int BUFFER_LEN = 256 * 1024;

static void sig_hdlr(int signum)
{
	switch (signum) {
	case SIGINT:
		do_exit = true;
		break;
	}
}

static void register_signals(void)
{
	signal(SIGINT, sig_hdlr);
}

static void get_version(void)
{
	DWORD dwVersion;

	FT_GetDriverVersion(NULL, &dwVersion);
	CMD_LOG("Driver version:%d.%d.%d\r\n", dwVersion >> 24,
			(uint8_t)(dwVersion >> 16), dwVersion & 0xFFFF);

	FT_GetLibraryVersion(&dwVersion);
	CMD_LOG("Library version:%d.%d.%d\r\n", dwVersion >> 24,
			(uint8_t)(dwVersion >> 16), dwVersion & 0xFFFF);
}

void* show_throughput(void* arg)
{
    (void)(FT_HANDLE)arg;

    struct timespec next;
#if defined(__APPLE__)
    // macOS: use nanosleep in a loop
    next.tv_sec = 1;      // interval in seconds
    next.tv_nsec = 0;
#else
    // Linux/other POSIX: use clock_gettime + clock_nanosleep
    clock_gettime(CLOCK_MONOTONIC, &next);  // Get current time
    next.tv_sec += 1;
#endif

    while (!do_exit) {
#if defined(__APPLE__)
        nanosleep(&next, NULL);
#else
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        next.tv_sec += 1;  // increment for next absolute time sleep
#endif

        int tx = atomic_exchange(&tx_count, 0);
        int rx = atomic_exchange(&rx_count, 0);

        CMD_LOG("TX: %.2f MiB/s RX: %.2f MiB/s, total: %.2f MiB\r\n",
                (float)tx / 1000000, (float)rx / 1000000,
                (float)(tx + rx) / 1000000);
    }

    return NULL;
}

void* write_test(void* arg) {
    FT_HANDLE handle = (FT_HANDLE)arg;
    uint8_t* buf = (uint8_t*)malloc(BUFFER_LEN);
    if (!buf) return NULL;

    while (!do_exit) {
        for (uint8_t channel = 0; channel < out_ch_cnt; channel++) {
            ULONG count = 0;
            if (FT_OK != FT_WritePipeEx(handle, channel, buf, BUFFER_LEN, &count, 1000)) {
                do_exit = 1;
                break;
            }
            tx_count += count;  // Increment the transmitted byte count
        }
    }

    CMD_LOG("Write stopped\r\n");
    free(buf);
    return NULL;
}

void* read_test(void* arg) {
    FT_HANDLE handle = (FT_HANDLE)arg;
    uint8_t* buf = (uint8_t*)malloc(BUFFER_LEN);
    if (!buf) return NULL;

    while (!do_exit) {
        for (uint8_t channel = 0; channel < in_ch_cnt; channel++) {
            ULONG count = 0;
            if (FT_OK != FT_ReadPipeEx(handle, channel, buf, BUFFER_LEN, &count, 1000)) {
                do_exit = 1;
                break;
            }
            rx_count += count;  // Increment the received byte count
        }
    }

    CMD_LOG("Read stopped\r\n");
    free(buf);
    return NULL;
}

BOOL SyncStreamingTest(void) 
{
    FT_STATUS ftStatus = FT_OK;
    get_version();

    out_ch_cnt = 1;  // Number of output channels
    in_ch_cnt = 1;   // Number of input channels

    FT_HANDLE handle = NULL;
    FT_Create(0, FT_OPEN_BY_INDEX, &handle);
    if (!handle) {
        CMD_LOG("Failed to create device\r\n");
        return -1;
    }

    FT_SetPipeTimeout(handle, 0x02, 0);  // Set pipe timeouts
    FT_SetPipeTimeout(handle, 0x82, 0);

    ftStatus = FT_SetStreamPipe(handle, TRUE, TRUE, 0, 1024 * 1024);
    if (FT_FAILED(ftStatus)) {
        CMD_LOG("FT_SetStreamPipe failed!=%d\n", ftStatus);
        goto _Exit;
    }

    // Create threads for reading, writing, and throughput measurement
    if (out_ch_cnt)
        pthread_create(&write_thread, NULL, write_test, handle);
    if (in_ch_cnt)
        pthread_create(&read_thread, NULL, read_test, handle);
    pthread_create(&measure_thread, NULL, show_throughput, handle);

    register_signals();

    // Join threads after testing
    if (out_ch_cnt)
        pthread_join(write_thread, NULL);
    if (in_ch_cnt)
        pthread_join(read_thread, NULL);
    pthread_join(measure_thread, NULL);

_Exit:
    // Clean up
    FT_ClearStreamPipe(handle, FALSE, FALSE, 0x02);
    FT_ClearStreamPipe(handle, FALSE, FALSE, 0x82);
    FT_Close(handle);
    return 0;
}


