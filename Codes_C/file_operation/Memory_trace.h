#ifndef __MEMORY_TRACE_H
#define __MEMORY_TRACE_H
#include <stdint.h>
#include "DataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void collect_unique_address_at_pagesize(MemoryRequestType *_memoryrequest, uint64_t _length, BufferType64bit *_buffer, TraceType _trace);
	uint64_t calculate_memory_footprint(BenchmarkType *_benchmark);

#ifdef __cplusplus
}
#endif

#endif