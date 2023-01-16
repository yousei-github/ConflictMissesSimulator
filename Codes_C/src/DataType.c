#include "DataType.h"
#include "Configuration.h"

// remember to free the allocated memory
void benchmark_deallocation(BenchmarkType* _benchmark)
{
    free(_benchmark->memorytrace);
    free(_benchmark->total_page);
}
