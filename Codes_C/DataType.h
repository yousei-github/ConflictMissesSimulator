#ifndef __DATATYPE_H
#define __DATATYPE_H

#include <stdint.h> 


typedef struct
{
	uint64_t address;		 
	uint8_t  type;  // Read: 0, Write: 1					
}MemoryRequestType;




#endif 