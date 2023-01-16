#ifndef __FILE_OPERATION_H
#define __FILE_OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DataType.h"

#define FILEPATHTYPE_LENGTH (80)
#define FILEPATH_LENGTH (19)

#ifdef __cplusplus
extern "C"
{
#endif

    typedef char FilePathType[FILEPATHTYPE_LENGTH];

    extern FilePathType FilePath[FILEPATH_LENGTH];

    MemoryRequestType* read_csv_to_get_memory_trace(FilePathType* _filepath, BenchmarkType* _benchmark);
    MemoryRequestType* read_binary_to_get_memory_trace(const char* _filepath, BenchmarkType* _benchmark, TraceType _trace);
    void read_binary_to_get_unique_memory_trace(BenchmarkType* _benchmark);
    void read_binary_to_get_segment_address_for_memory_trace(BenchmarkType* _benchmark);
    void create_binary_to_store_unique_memory_address(BenchmarkType* _benchmark);
    void create_binary_to_store_memory_trace_with_segment_address(BenchmarkType* _benchmark);

#ifdef __cplusplus
}
#endif

#endif