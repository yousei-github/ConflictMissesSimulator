#ifndef __MEMORY_TRACE_H
#define __MEMORY_TRACE_H
#include <stdint.h>
#include "DataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		byte = 0,
		page = 1,
	} TraceType;

	void collect_unique_address(MemoryRequestType *_memoryrequest, uint64_t _length, BufferType64bit *_buffer);
	uint64_t calculate_memory_footprint(BenchmarkType *_benchmark, TraceType _trace);

#ifdef __cplusplus
}
#endif

#endif