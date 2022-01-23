#ifndef __MEMORY_TRACE_H
#define __MEMORY_TRACE_H
#include <stdint.h>
#include "DataType.h"

void collect_unique_address(MemoryRequestType* _memoryrequest, uint64_t _length, BufferType64bit *_buffer);
uint64_t calculate_memory_footprint(BenchmarkType *_benchmark);

#endif