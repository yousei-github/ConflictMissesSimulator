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

    free(benchmark_600.memorytrace); // remember to free the allocated memory
    free(benchmark_600.totalpage);   // remember to free the allocated memory

    printf("End.\n---------------------------------------\n\n");
    return 0;
}