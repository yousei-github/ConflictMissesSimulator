#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#define OS              (WIN32)
#define THRESHOLD       (1)
#define TEST_BENCHMARK  (5)
#define OUTPUT_DETAIL   (ENABLE)
#define USE_OPENMP      (DISABLE)
#define TEST_MODE       (DISABLE)

// create files
#define CREATE_UNIQUE_ADDRESS_FILE         (DISABLE)
#define CREATE_TRACE_FILE_WITH_PAGE_NUMBER (DISABLE)

// read files
#define READ_UNIQUE_ADDRESS_FILE           (ENABLE)
#define READ_TRACE_FILE_WITH_PAGE_NUMBER   (DISABLE)

#define DECREASE_COUNTER                   (DISABLE)
#define LIMIT_TRACE_READ_NUMBER            (ENABLE)

#define OUTPUT_CSV_NAME ("_data.csv")

// note here
#if LIMIT_TRACE_READ_NUMBER == ENABLE
#define MAX_TRACE_READ_NUMBER (1000000)
#endif

#if USE_OPENMP == ENABLE
#define NUMBER_THREADS (6)
#endif

#define WIN32 1
#define LINUX 2

#define ENABLE (1)
#define DISABLE (0)

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#if (OS == WIN32)
#include <time.h>
#elif (OS == LINUX)
#include <sys/time.h>
#endif

#if USE_OPENMP == ENABLE
#include <omp.h>
#endif

// User files
#include "DataType.h"
#include "File_operation.h"
#include "Memory_trace.h"
#include "Memory_structure.h"
#include "Hybrid_simulation.h"
#include "user_math.h"

#endif
