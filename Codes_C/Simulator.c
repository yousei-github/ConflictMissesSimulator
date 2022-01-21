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
    if(fd1 == NULL)
	{
		printf("VERIFY Error: Open file .csv failed.\n");
		exit(1);
	}
    fseek(fd1, 0, SEEK_END);    // Go to the end of this file
    length = ftell(fd1);
    printf("Open file .csv successfully. length = %lld B or %lld KB\n", length, length / 1024);

    fseek(fd1, 0, SEEK_SET);    // Go to the beginning of this file
    uint8_t Buffer[28];
    uint64_t num1 = fread(Buffer, sizeof(char), 28, fd1);
    for (uint8_t i = 0; i < 28; i++)
    {
        printf("%c", Buffer[i]);
    }
    printf("\n");
    for (uint8_t i = 0; i < 28; i++)
    {
        printf("%d ", Buffer[i]);
    }
    printf("\n");

    fclose(fd1);


    printf("End.\n---------------------------------------\n\n");
    return 0;
}