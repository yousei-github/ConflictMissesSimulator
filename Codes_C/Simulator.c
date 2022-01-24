#include "Configuration.h"

/* Declaration */

int main(int argc, char *argv[])
{
    printf("\n---------------------------------------\nFunction is %s.\n", argv[0]);
    if (argc >= 2)
    { /* if an argument is specified */
        printf("if an argument is specified");
    }
    BenchmarkType benchmark_600;
    read_csv_to_get_memory_trace(&FilePath[0], &benchmark_600);

    calculate_memory_footprint(&benchmark_600);

    MemoryStructureType *memorystructure = memory_structure_initialization(&benchmark_600, Ratio_1_to_4, Direct_Mapped, 8);

    hybrid_direct_mapped_simulation(&benchmark_600, memorystructure);

    free(benchmark_600.memorytrace);                // remember to free the allocated memory
    free(benchmark_600.totalpage);                  // remember to free the allocated memory
    memory_structure_deallocation(memorystructure); // remember to free the allocated memory

    for (uint16_t i = 1; i < FILEPATH_LENGTH; i++)
    {
        if (i == 13)
        {
            continue;   // 13 is not good
        }
        
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark_600);
        calculate_memory_footprint(&benchmark_600);
        memorystructure = memory_structure_initialization(&benchmark_600, Ratio_1_to_4, Direct_Mapped, 8);
        hybrid_direct_mapped_simulation(&benchmark_600, memorystructure);

        free(benchmark_600.memorytrace);                // remember to free the allocated memory
        free(benchmark_600.totalpage);                  // remember to free the allocated memory
        memory_structure_deallocation(memorystructure); // remember to free the allocated memory
    }

    printf("End.\n---------------------------------------\n\n");
    return 0;
}