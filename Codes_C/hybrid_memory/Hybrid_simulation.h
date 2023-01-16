#ifndef __HYBRID_SIMULATION_H
#define __HYBRID_SIMULATION_H
#include <stdint.h>
#include "DataType.h"
#include "Memory_structure.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void hybrid_simulation_usingLRU(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);

#ifdef __cplusplus
}
#endif

#endif
