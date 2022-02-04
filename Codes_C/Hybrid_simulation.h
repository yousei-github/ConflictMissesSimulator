#ifndef __HYBRID_SIMULATION_H
#define __HYBRID_SIMULATION_H
#include <stdint.h>
#include "DataType.h"
#include "Memory_structure.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void hybrid_direct_mapped_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);
    void hybrid_set_associative_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);
    void hybrid_fully_associative_simulation_usingqueue(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);
    void hybrid_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);
    void hybrid_simulation2(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);
    void hybrid_simulation3(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure);

#ifdef __cplusplus
}
#endif

#endif
