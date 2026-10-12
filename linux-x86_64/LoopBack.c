/*
 * File: LoopBack.c
 * Description:
 *     This program performs a high-precision USB loopback throughput test
 *     using the FTDI FT600/FT601 device via the FTDI D3XX driver API.
 *
 *     The application repeatedly writes a block of data to the device's
 *     FIFO pipe and reads the same amount back, measuring:
 *         - Minimum loop time per transaction
 *         - Maximum loop time
 *         - Average time over N iterations
 *         - Total accumulated time
 *
 *     Multiple buffer sizes are tested to compare throughput performance
 *     under different load conditions. Each write+read cycle is timed using
 *     CLOCK_MONOTONIC for nanosecond-resolution measurements.
 *
 * Features:
 *     • Uses FT_WritePipeEx() and FT_ReadPipeEx() for data transfers  
 *     • Supports configurable buffer sizes  
 *     • Runs 1000 loopback iterations per buffer size  
 *     • Reports detailed timing statistics in a formatted table  
 *     • Converts throughput metrics to human-readable units (MiB, Mbit/s)  
 *
 * Requirements:
 *     • FTDI D3XX driver library (ftd3xx.h)  
 *     • FT600/FT601 device in synchronous FIFO mode  
 *
 * Author: <FTDI Chip>
 * Created: <2025>
 */

#define _POSIX_C_SOURCE 200809L
#include <time.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include "ftd3xx.h"

#define FIFO_CHANNEL_1  0
#define TIMEOUT         0
#define NUM_ITERATIONS  1000

// Time difference in nanoseconds
long long time_diff_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}

// Convert bytes to MiB (Mebibytes - binary units)
double bytes_to_MiB(uint64_t bytes) {
    return (double)bytes / (1024.0 * 1024.0);
}

// Convert bytes/sec to Mbits/sec (decimal)
double bytes_per_sec_to_Mbits(uint64_t bytes, double seconds) {
    return (bytes * 8.0) / seconds / 1e6;
}

// Convert bytes/sec to MiB/sec (binary)
double bytes_per_sec_to_MiB(uint64_t bytes, double seconds) {
    return bytes / seconds / (1024.0 * 1024.0);
}

// Convert Mbits to MiB
double Mbits_to_MiB(double mbits) {
    return mbits / 8.388608;
}

typedef struct {
    uint32_t buffer_size;
    double min_time_ms;
    double max_time_ms;
    double avg_time_ms;
    double total_time_ms;
} TestResult;

void run_throughput_test(FT_HANDLE handle, uint32_t buffer_size, TestResult *result) {
    struct timespec loop_start, loop_end;
    DWORD count;
    int success_count = 0;
    long long total_loop_time_ns = 0;
    long long min_time_ns = LLONG_MAX;
    long long max_time_ns = 0;

    uint8_t *out_buf = calloc(1, buffer_size);
    uint8_t *in_buf  = calloc(1, buffer_size);
    
    if (!out_buf || !in_buf) {
        printf("Memory allocation failed for %u bytes!\n", buffer_size);
        free(out_buf);
        free(in_buf);
        return;
    }

    // Run loop 1000 times, measuring each complete write+read cycle
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // ============ START TIMING FOR THIS LOOP ============
        clock_gettime(CLOCK_MONOTONIC, &loop_start);

        // ---- WRITE ----
        if (FT_OK != FT_WritePipeEx(handle, FIFO_CHANNEL_1, out_buf, buffer_size, &count, TIMEOUT)) {
            printf("Write Failed at iteration %d for %u bytes!\n", i + 1, buffer_size);
            goto cleanup;
        }

        // ---- READ ----
        if (FT_OK != FT_ReadPipeEx(handle, FIFO_CHANNEL_1, in_buf, buffer_size, &count, TIMEOUT)) {
            printf("Read Failed at iteration %d for %u bytes!\n", i + 1, buffer_size);
            goto cleanup;
        }

        // ============ END TIMING FOR THIS LOOP ============
        clock_gettime(CLOCK_MONOTONIC, &loop_end);

        long long loop_time_ns = time_diff_ns(loop_start, loop_end);
        total_loop_time_ns += loop_time_ns;

        // Track min and max times
        if (loop_time_ns < min_time_ns) min_time_ns = loop_time_ns;
        if (loop_time_ns > max_time_ns) max_time_ns = loop_time_ns;

        success_count++;
    }

    // ============ STORE RESULTS ============
    result->buffer_size = buffer_size;
    result->min_time_ms = min_time_ns / 1e6;
    result->max_time_ms = max_time_ns / 1e6;
    result->avg_time_ms = (double)total_loop_time_ns / success_count / 1e6;
    result->total_time_ms = total_loop_time_ns / 1e6;

cleanup:
    free(in_buf);
    free(out_buf);
}

int main(int argc, char *argv[])
{
    FT_HANDLE handle = NULL;
    unsigned short vendorId, productId;
    uint32_t buffer_sizes[] = {512, 1024, 2048, 4096, 8192};
    int num_sizes = sizeof(buffer_sizes) / sizeof(buffer_sizes[0]);
    TestResult results[5];

    // Suppress unused parameter warnings
    (void)argc;
    (void)argv;

    FT_STATUS ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &handle);
    if (ftStatus != FT_OK || !handle) {
        printf("Failed to create device!\n");
        return -1;
    }

    printf("Device created successfully\n");

    FT_GetVIDPID(handle, &vendorId, &productId);
    printf("Vendor ID: 0x%04X  Product ID: 0x%04X\n", vendorId, productId);

    FT_SetPipeTimeout(handle, 0x02, 0);
    FT_SetPipeTimeout(handle, 0x82, 0);

    printf("\nRunning throughput tests (%d iterations per buffer size)...\n\n", NUM_ITERATIONS);

    // Run tests for all buffer sizes
    for (int i = 0; i < num_sizes; i++) {
        run_throughput_test(handle, buffer_sizes[i], &results[i]);
    }

    // Print table header
    printf("+----------------------+-------------+----------+----------+----------+------------+\n");
    printf("| Number of Iterations | Buffer Size | Min Time | Max Time | Avg Time | Total Time |\n");
    printf("+----------------------+-------------+----------+----------+----------+------------+\n");

    // Print each row
    for (int i = 0; i < num_sizes; i++) {
        printf("| %20d | %11u | %8.3f | %8.3f | %8.3f | %10.3f |\n",
               NUM_ITERATIONS,
               results[i].buffer_size,
               results[i].min_time_ms,
               results[i].max_time_ms,
               results[i].avg_time_ms,
               results[i].total_time_ms);
    }

    printf("+----------------------+-------------+----------+----------+----------+------------+\n");

    FT_Close(handle);
    return 0;
}