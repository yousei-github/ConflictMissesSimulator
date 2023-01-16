#ifndef __DATATYPE_H
#define __DATATYPE_H
#include <stdint.h>

#define KB (1024)		   // unit is Byte
#define CACHE_LINE (64)	   // unit is Byte
#define PAGE_SIZE (4 * KB) // unit is Byte
#define PAGE_SIZE_AT_CACHE_LINE (4 * KB / 64)
#define FAST_MEMORY_CAPACITY (4 * KB * KB)
#define CACHE_LINE_NUMBER_IN_PAGE (PAGE_SIZE / CACHE_LINE)

#define LOG2_PAGE_SIZE_AT_CACHE_LINE (6)

#define FILE_NAME_LENGTH (128)
#define DIRECTORY_NUMBER (50)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		cache_line = 0,
		page = 1,
	} TraceType;

	typedef struct
	{
		uint64_t address;
		uint64_t segment_address;
		uint8_t type; // Read: 0, Write: 1
	} MemoryRequestType;

	typedef struct
	{
		uint64_t base;
		// uint64_t page_number; //the index is same as page_number (it is not needed for TotalPageType)
	} TotalPageType; // for single page

	typedef struct
	{
		char file_name[FILE_NAME_LENGTH];
		MemoryRequestType *memorytrace;
		uint64_t tracelength; // for MemoryRequestType *memorytrace
		TraceType trace_type;
		TotalPageType *total_page;	// a new address space (continual from 0 to N), the index is same as page_number
		uint64_t total_page_number; // for TotalPageType *total_page, it is used to calculate the program's memory footprint.
		uint64_t address_space;		// the address space used at byte unit
		float hit_rate;
		float conflict_miss_rate;
		uint64_t migration_data_NM_to_FM_in_byte; // NM: near memory, FM: far memory
		uint64_t unuse_migrated_cache_line_NM_number;
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

	typedef enum
	{
		clear = 0,
		set = 1,
	} AccessType;

	void benchmark_deallocation(BenchmarkType *_benchmark);

#ifdef __cplusplus
}
#endif

#endif