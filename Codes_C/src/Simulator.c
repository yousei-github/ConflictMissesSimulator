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

#if (OS == WIN32)
double get_elapsed_time_in_microseconds(clock_t* begin, clock_t* end);
#elif (OS == LINUX)
double get_elapsed_time_in_microseconds(struct timeval* begin, struct timeval* end);
#endif

int main(int argc, char* argv[])
{
    printf("\n---------------------------------------\n(%s) Function is %s.\n", __func__, argv[0]);
    if (argc >= 2)
    { /* if the argument is specified */
        printf("(%s) The argument is specified:\n", __func__);
        for (uint8_t i = 1; i < argc; i++)
        {
            printf("argv[%d] is %s.\n", i, argv[i]);
        }
    }
    else
    {
        printf("(%s) Not enough argument.\n", __func__);
        exit(1);
    }
    coldqueue_initialization(0);
    hotqueue_initialization(0);

    BenchmarkType benchmark;
    MemoryStructureType* memorystructure;

#if USE_OPENMP == ENABLE
#pragma omp parallel
    {
        printf("(%s) Thread %d of %d threads says hello.\n", __func__, omp_get_thread_num(), omp_get_num_threads());
    }
#endif

#if (OS == WIN32)
    clock_t start, stop;
    start = clock();
#elif (OS == LINUX)
    struct timeval start, stop;
    gettimeofday(&start, NULL);
#endif

    for (uint8_t i = 1; i < argc; i++)
    {
        printf("\n(%s) Benchmark: %s.\n", __func__, argv[i]);

        read_binary_to_get_memory_trace(argv[i], &benchmark, cache_line);

        calculate_memory_footprint(&benchmark);
        //         double us;
        // #if (OS == WIN32)
        //         stop = clock();
        //         us = get_elapsed_time_in_microseconds(&start, &stop);
        // #endif
        //         printf("(%s) Elapsed time: %.3lf second.\n", __func__, us / 1000000.0);

        // open a csv file for storing the simulation results
        char file_name[FILE_NAME_LENGTH];
        strcpy(file_name, benchmark.file_name);
        strcat(file_name, OUTPUT_CSV_NAME);
        printf("(%s) file_name is %s.\n", __func__, file_name);
        FILE* fd1 = fopen(file_name, "w");
        if (fd1 == NULL)
        {
            printf("(%s) Error: Open csv file failed.\n", __func__);
            exit(1);
        }

        fprintf(fd1, "%s,%lld,%lld\n", argv[i], benchmark.address_space, benchmark.total_page_number * PAGE_SIZE);

        for (uint16_t index_associative = 0; index_associative < MAX_SETASSOCIATIVE_TYPE; index_associative++)
        {
            SetAssociativeType set_associative = Direct_Mapped;
            switch (index_associative)
            {
            case 0:
                set_associative = Direct_Mapped;
                break;
            case 1:
                set_associative = Two_Way;
                break;
            case 2:
                set_associative = Four_Way;
                break;
            case 3:
                set_associative = Eight_Way;
                break;
            case 4:
                set_associative = Fully_Associative;
                break;
            default:
                break;
            }

            for (uint16_t index_granularity = 0; index_granularity < MAX_MIGRATIONGRANULARITY_TYPE; index_granularity++)
            {
                MigrationGranularityType granularity = Byte_64;
                switch (index_granularity)
                {
                case 0:
                    granularity = Byte_64;
                    break;
                case 1:
                    granularity = Byte_128;
                    break;
                case 2:
                    granularity = Byte_256;
                    break;
                case 3:
                    granularity = Byte_512;
                    break;
                case 4:
                    granularity = Byte_1024;
                    break;
                case 5:
                    granularity = Byte_2048;
                    break;
                case 6:
                    granularity = Byte_4096;
                    break;
                default:
                    break;
                }

                memorystructure = memory_structure_initialization(&benchmark, FAST_MEMORY_CAPACITY, set_associative, THRESHOLD, granularity);

                hybrid_simulation_usingLRU(&benchmark, memorystructure);

                // remember to free the allocated memory
                memory_structure_deallocation(memorystructure);

                fprintf(fd1, "%d,%d,%f,%lld,%lld\n", set_associative, granularity, benchmark.hit_rate, benchmark.migration_data_NM_to_FM_in_byte, benchmark.unuse_migrated_cache_line_NM_number);
            }
            fprintf(fd1, "\n");
        }
        // remember to free the allocated memory
        benchmark_deallocation(&benchmark);

        fprintf(fd1, "\n");
        fclose(fd1);
    }

    double us;
#if (OS == WIN32)
    stop = clock();
    us = get_elapsed_time_in_microseconds(&start, &stop);
#elif (OS == LINUX)
    gettimeofday(&stop, NULL);
    us = get_elapsed_time_in_microseconds(&start, &stop);
#endif
    printf("(%s) Elapsed time: %.3lf second.\n", __func__, us / 1000000.0);

    printf("(%s) End.\n---------------------------------------\n\n", __func__);
    return 0;
}

#if (OS == WIN32)
/* in microseconds (us) */
double get_elapsed_time_in_microseconds(clock_t* begin, clock_t* end)
{
    return ((double)(*end - *begin)) / CLOCKS_PER_SEC * 1000000;
}
#elif (OS == LINUX)
/* in microseconds (us) */
double get_elapsed_time_in_microseconds(struct timeval* begin, struct timeval* end)
{
    return (end->tv_sec - begin->tv_sec) * 1000000 + (end->tv_usec - begin->tv_usec);
}
#endif
