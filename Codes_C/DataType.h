#ifndef __DATATYPE_H
#define __DATATYPE_H
#include <stdint.h> 

#define KB (1024)	// unit is Byte
#define PAGE_SIZE (4*KB)

typedef struct
{
	uint64_t address;
	uint64_t page_number;		 
	uint8_t  type;  // Read: 0, Write: 1					
}MemoryRequestType;

typedef struct
{
	MemoryRequestType* memorytrace;
	uint64_t length;
}BenchmarkType;

typedef struct
{
	uint64_t* buffer;
	uint64_t length;
}BufferType64bit;

#endif 