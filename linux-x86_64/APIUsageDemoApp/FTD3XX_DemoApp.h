#ifndef FTD3XX_DEMOAPP_H
#define FTD3XX_DEMOAPP_H

/*file: FTD3XX_DemoApp.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <stdint.h>
#include <stdbool.h>
#include "ftd3xx.h"
#include "Types.h"

#define CMD_LOG(...) printf(__VA_ARGS__)


/* Forward declarations for the test functions used in the test array */
 BOOL OpenCloseTest(void);
 BOOL ModifyChipConfigurationTest(void);
 BOOL DescriptorTest(void);
#if LOOPBACK_MODE
 BOOL ReadPipeExWritePipeExLoopbackTest(void);
 BOOL LoopbackTest(void);
 BOOL AsyncLoopbackTest(void);
 BOOL MultiAsyncLoopbackTest(void);
 BOOL NotificationDataTest(void);
 BOOL GPIOTest(void);
 BOOL SetChipConfigurationTest(void);
 BOOL ResetChipConfigurationTest(void);
 BOOL AsyncReadWriteLoopbackTest(void);
 BOOL AbortRecovery(void);
#else
BOOL SyncStreamingTest(void);
#endif /*STREAMING_ON */

#endif /*FTD3XX_DEMOAPP_H*/