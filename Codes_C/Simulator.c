#include "Configuration.h"

/* Declaration */

int main(int argc, char *argv[])
{
    printf("\n---------------------------------------\nFunction is %s.\n", argv[0]);
    if (argc >= 2)
    { /* if an argument is specified */
        printf("if an argument is specified");
    }
    BenchmarkType benchmark;
    MemoryStructureType *memorystructure;

    for (uint16_t i = 0; i < FILEPATH_LENGTH; i++)
    //uint16_t i = 12;
    {
        printf("Benchmark: %d\n", i + 1);
        // Direct_Mapped
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Direct_Mapped, THRESHOLD);
        hybrid_direct_mapped_simulation(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);

        // Two_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Two_Way, THRESHOLD);
        hybrid_set_associative_simulation(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);

        // Four_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Four_Way, THRESHOLD);
        hybrid_set_associative_simulation(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);

        // Eight_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Eight_Way, THRESHOLD);
        hybrid_set_associative_simulation(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);

        // Fully_Associative
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Fully_Associative, THRESHOLD);
        hybrid_set_associative_simulation(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
    }

    printf("End.\n---------------------------------------\n\n");
    return 0;
}