#ifndef __DATATYPE_H
#define __DATATYPE_H
#include <stdint.h>

#define KB (1024) // unit is Byte
#define PAGE_SIZE (4 * KB)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		uint64_t address;
		uint64_t page_number;
		uint8_t type; // Read: 0, Write: 1
	} MemoryRequestType;

	typedef struct
	{
		uint64_t base;
		uint64_t page_number;
	} TotalPageType;

	typedef struct
	{
		MemoryRequestType *memorytrace;
		uint64_t tracelength;	   // for memorytrace
		TotalPageType *totalpage;  // new address space (continual)
		uint64_t totoalpagelength; // for totalpage
		float hit_rate;
		float conflictmiss_rate;
	} BenchmarkType;

	typedef struct
	{
		uint64_t *buffer;
		uint64_t length;
	} BufferType64bit;

	typedef enum
	{
		cold = 0,
		hot = 1,
	} HotnessType;

	typedef enum
	{
		slow = 0,
		fast = 1,
	} LatencyType;

	void benchmark_deallocation(BenchmarkType *_benchmark);

#ifdef __cplusplus
}
#endif

#endif