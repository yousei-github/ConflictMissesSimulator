#include "Configuration.h"

/* Declaration */

int main(int argc, char *argv[])
{
    printf("\n---------------------------------------\nFunction is %s.\n", argv[0]);
    if (argc >= 2)
    { /* if an argument is specified */
        printf("if an argument is specified");
    }

    uint64_t length = 0;
    //Read one .csv ile
    FILE *fd1 = fopen("..\\memory_traces_50000\\600.perlbench_s-210B.champsimtrace.xz_DRAM_addr.csv", "r");
    if (fd1 == NULL)
    {
        printf("VERIFY Error: Open file .csv failed.\n");
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    length = ftell(fd1);
    printf("Open file .csv successfully. length = %lld B or %lld KB\n", length, length / 1024);

    // Read .csv file's first 30 bytes and see what they are
    // fseek(fd1, 0, SEEK_SET);    // Go to the beginning of this file
    // uint8_t Buffer[28];
    // uint64_t num1 = fread(Buffer, sizeof(char), 28, fd1);
    // for (uint8_t i = 0; i < 28; i++)
    // {
    //     printf("%c", Buffer[i]);
    // }
    // printf("\n");
    // for (uint8_t i = 0; i < 28; i++)
    // {
    //     printf("%d ", Buffer[i]);
    // }
    // printf("\n");
    // /*
    // 68179700151,0
    // 68179700151,0
    // ->
    // 54 56 49 55 57 55 48 48 49 53 49 44 48 10 54 56 49 55 57 55 48 48 49 53 49 44 48 10
    // */

    fseek(fd1, 0, SEEK_SET); // Go to the beginning of this file
    uint64_t count = 0;
    while (1)
    {
        if (getc(fd1) == ',')
            count++;
        
        if (feof(fd1))
            break;
    }
    printf("count is %lld\n", count);

    MemoryRequestType* MemoryRequest = (MemoryRequestType *)malloc(sizeof(MemoryRequestType) * count);
    if (MemoryRequest == NULL)
	{
		printf("Error: Memory allocation failed.\n");
		exit(1);
	}
    //free(MemoryRequest);

    //
    
    fseek(fd1, 0, SEEK_SET); // Go to the beginning of this file
    for (uint64_t i = 0; i < count; i++)
    {
        uint16_t _count = 0;
        char string1[15];
        char _temp;
        while (1)
        {
            fread(&_temp, sizeof(char), 1, fd1);
            if (isdigit(_temp))
            {
                string1[_count++] = _temp;
            }
            else    // arrive ','
            {
                string1[_count] = '\0';
                break;
            }
        }
        //printf("%s\n", string1);
        MemoryRequest[i].address = atoll(string1);
        fread(&_temp, sizeof(char), 1, fd1);
        MemoryRequest[i].type = _temp - 48; // '0' - 48 = 0 and '1' - 48 = 1
        fread(&_temp, sizeof(char), 1, fd1);    // ignore '\n'
    }
    //printf("long int %ld\n", sizeof(long int));
    for (uint64_t i = 0; i < count; i++)
    {
        printf("%lld %lld, %d\n", i, MemoryRequest[i].address, MemoryRequest[i].type);
    }


    free(MemoryRequest);
    fclose(fd1);

    printf("End.\n---------------------------------------\n\n");
    return 0;
}