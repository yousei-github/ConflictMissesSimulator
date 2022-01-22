#include "Configuration.h"

/* Declaration */

int main(int argc, char *argv[])
{
    printf("\n---------------------------------------\nFunction is %s.\n", argv[0]);
    if (argc >= 2)
    { /* if an argument is specified */
        printf("if an argument is specified");
    }

    MemoryRequestType* MemoryRequest = read_csv_to_get_memory_trace(&FilePath[10]);
    


    free(MemoryRequest);

    printf("End.\n---------------------------------------\n\n");
    return 0;
}