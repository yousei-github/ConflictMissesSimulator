#include "Configuration.h"

/* Declaration */
extern void coldqueue_initialization(uint8_t number);
extern uint64_t coldqueue_size(uint8_t number);
extern uint8_t coldqueue_empty(uint8_t number);
extern void coldqueue_enqueue(uint8_t number, uint64_t element);
extern uint64_t coldqueue_dequeue(uint8_t number);
extern void hotqueue_initialization(uint8_t number);
extern uint64_t hotqueue_size(uint8_t number);
extern uint8_t hotqueue_empty(uint8_t number);
extern void hotqueue_enqueue(uint8_t number, uint64_t element);
extern uint64_t hotqueue_dequeue(uint8_t number);

int main(int argc, char *argv[])
{
    printf("\n---------------------------------------\nFunction is %s.\n", argv[0]);
    if (argc >= 2)
    { /* if an argument is specified */
        printf("if an argument is specified");
    }
    coldqueue_initialization(0);
    hotqueue_initialization(0);
    //hotqueue_initialization(1);

    FILE *fd1 = fopen("data.csv", "w");
    BenchmarkType benchmark;
    MemoryStructureType *memorystructure;

    for (uint16_t i = 0; i < FILEPATH_LENGTH; i++)
    //uint16_t i = 6;
    {
        printf("Benchmark: %d\n", i + 1);
        // Direct_Mapped
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Direct_Mapped, THRESHOLD);
        //hybrid_direct_mapped_simulation(&benchmark, memorystructure);
        //hybrid_simulation(&benchmark, memorystructure);
        //hybrid_simulation2(&benchmark, memorystructure);
        hybrid_simulation3(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
        //fprintf(fd1, "%f\n", benchmark.hit_rate);
        fprintf(fd1, "%f ", benchmark.hit_rate);

        // Two_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Two_Way, THRESHOLD);
        //hybrid_set_associative_simulation(&benchmark, memorystructure);
        //hybrid_simulation(&benchmark, memorystructure);
        //hybrid_simulation2(&benchmark, memorystructure);
        hybrid_simulation3(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
        //fprintf(fd1, "%f\n", benchmark.hit_rate);
        fprintf(fd1, "%f ", benchmark.hit_rate);

        // Four_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Four_Way, THRESHOLD);
        //hybrid_set_associative_simulation(&benchmark, memorystructure);
        //hybrid_simulation(&benchmark, memorystructure);
        //hybrid_simulation2(&benchmark, memorystructure);
        hybrid_simulation3(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
        //fprintf(fd1, "%f\n", benchmark.hit_rate);
        fprintf(fd1, "%f ", benchmark.hit_rate);

        // Eight_Way
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Eight_Way, THRESHOLD);
        //hybrid_set_associative_simulation(&benchmark, memorystructure);
        //hybrid_simulation(&benchmark, memorystructure);
        //hybrid_simulation2(&benchmark, memorystructure);
        hybrid_simulation3(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
        //fprintf(fd1, "%f\n", benchmark.hit_rate);
        fprintf(fd1, "%f ", benchmark.hit_rate);

        // Fully_Associative
        read_csv_to_get_memory_trace(&FilePath[i], &benchmark);
        calculate_memory_footprint(&benchmark, page);
        memorystructure = memory_structure_initialization(&benchmark, Ratio_1_to_4, Fully_Associative, THRESHOLD);
        //hybrid_set_associative_simulation(&benchmark, memorystructure);
        //hybrid_fully_associative_simulation_usingqueue(&benchmark, memorystructure);
        //hybrid_simulation(&benchmark, memorystructure);
        //hybrid_simulation2(&benchmark, memorystructure);
        hybrid_simulation3(&benchmark, memorystructure);
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);
        memory_structure_deallocation(memorystructure);
        //fprintf(fd1, "%f\n\n", benchmark.hit_rate);
        fprintf(fd1, "%f; ", benchmark.hit_rate);

        coldqueue_initialization(0);
        hotqueue_initialization(0);
    }

    fclose(fd1);
    printf("End.\n---------------------------------------\n\n");
    return 0;
}