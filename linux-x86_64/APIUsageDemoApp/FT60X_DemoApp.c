/*
 * FT600 API Usage Demo App - Linux C port
 *
 * Converted from Windows C++ demo to Linux-compatible C.
 *
 * Notes:
 * - Assumes FTDI test function implementations are available in FTD3XX_Test.c.
 * - Uses direct CMD_LOG logging.
 *
 * Build suggestion:
 * gcc -std=c11 -Wall -Wextra -o FT60X_DemoApp FT60X_DemoApp.c FTD3XX_Test.o -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define USE_245_MODE 1
#define LOOPBACK_MODE 1

#include "FTD3XX_DemoApp.h"

#define TRUE 1
#define FALSE 0
typedef unsigned int DWORD;
typedef unsigned long long ULONGLONG;

typedef struct {
    const char* name;
    BOOL result;
} TestResult;

int main(void)
{
    CMD_LOG("FT60X Demo App - Starting...\n");

    TestResult tests[20];
    int testIndex = 0;

    CMD_LOG("--- Starting OpenCloseTest ---\n");
    tests[testIndex++] = (TestResult){ "OpenCloseTest", OpenCloseTest() };
    CMD_LOG("--- END OpenCloseTest ---\n\n");

    CMD_LOG("--- Starting GPIOTest ---\n");
    tests[testIndex++] = (TestResult){ "GPIOTest", GPIOTest() };
    CMD_LOG("--- END GPIOTest ---\n\n");

    CMD_LOG("--- Starting DescriptorTest ---\n");
    tests[testIndex++] = (TestResult){ "DescriptorTest", DescriptorTest() };
    CMD_LOG("--- END DescriptorTest ---\n\n");

#if LOOPBACK_MODE
    CMD_LOG("-------------------- LoopBack Mode -----------------------\n");

    CMD_LOG("--- Starting LoopbackTest ---\n");
    tests[testIndex++] = (TestResult){ "LoopbackTest", LoopbackTest() };
    CMD_LOG("--- END LoopbackTest ---\n\n");

    CMD_LOG("--- Starting AsyncLoopbackTest ---\n");
    tests[testIndex++] = (TestResult){ "AsyncLoopbackTest", AsyncLoopbackTest() };
    CMD_LOG("--- END AsyncLoopbackTest ---\n\n");

    CMD_LOG("--- Starting MultiAsyncLoopbackTest ---\n");
    tests[testIndex++] = (TestResult){ "MultiAsyncLoopbackTest", MultiAsyncLoopbackTest() };
    CMD_LOG("--- END MultiAsyncLoopbackTest ---\n\n");

    CMD_LOG("--- Starting AsyncReadWriteLoopbackTest ---\n");
    tests[testIndex++] = (TestResult){ "AsyncReadWriteLoopbackTest", AsyncReadWriteLoopbackTest() };
    CMD_LOG("--- END AsyncReadWriteLoopbackTest ---\n\n");

    CMD_LOG("--- Starting NotificationDataTest ---\n");
    tests[testIndex++] = (TestResult){ "NotificationDataTest", NotificationDataTest() };
    CMD_LOG("--- END NotificationDataTest ---\n\n");

    CMD_LOG("--- Starting ModifyChipConfigurationTest ---\n");
    tests[testIndex++] = (TestResult){ "ModifyChipConfigurationTest", ModifyChipConfigurationTest() };
    CMD_LOG("--- END ModifyChipConfigurationTest ---\n\n");
    sleep(3);
    CMD_LOG("--- Starting ResetChipConfigurationTest ---\n");
    tests[testIndex++] = (TestResult){ "ResetChipConfigurationTest", ResetChipConfigurationTest() };
    CMD_LOG("--- END ResetChipConfigurationTest ---\n\n");

    sleep(3);

    CMD_LOG("--- Starting AbortRecovery ---\n");
    tests[testIndex++] = (TestResult){ "AbortRecovery", AbortRecovery() };
    CMD_LOG("--- END AbortRecovery ---\n\n");

    CMD_LOG("-------------------- LoopBack Mode END -----------------------\n");

#else
    CMD_LOG("-------------------- Streaming Mode Start -----------------------\n");
    tests[testIndex++] = (TestResult){ "SyncStreamingTest", SyncStreamingTest() };
    CMD_LOG("-------------------- Streaming Mode End -----------------------\n");
#endif

    CMD_LOG("FT60X Demo App - Exiting.\n");

    // --- Summary Report ---
    CMD_LOG("\n==================== Test Summary ====================\n");
    for (int i = 0; i < testIndex; ++i) {
        CMD_LOG("%-35s : %s\n", tests[i].name, tests[i].result ? "PASS" : "FAIL");
    }
    CMD_LOG("======================================================\n");

    CMD_LOG("Press Enter to exit...\n");
    (void)getchar();

    return 0;
}
