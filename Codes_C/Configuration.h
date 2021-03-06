#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#define OS (WIN32)
#define THRESHOLD (4)
#define TEST_BENCHMARK (5)
#define OUTPUT_DETAIL (DISABLE)

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

#if (OS == WIN32)
#include <time.h>
#elif (OS == LINUX)
#include <sys/time.h>
#endif

// User files
#include "DataType.h"
#include "File_operation.h"
#include "Memory_trace.h"
#include "Memory_structure.h"
#include "Hybrid_simulation.h"

#endif
