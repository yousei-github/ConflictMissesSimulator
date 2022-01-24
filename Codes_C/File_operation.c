#include "File_operation.h"
#include <stdio.h>

FilePathType FilePath[19] = {
    "..\\memory_traces_50000\\600.perlbench_s-210B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\602.gcc_s-734B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\603.bwaves_s-3699B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\605.mcf_s-665B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\607.cactuBSSN_s-2421B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\619.lbm_s-4268B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\620.omnetpp_s-874B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\621.wrf_s-575B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\623.xalancbmk_s-700B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\625.x264_s-18B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\627.cam4_s-573B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\628.pop2_s-17B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\631.deepsjeng_s-928B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\638.imagick_s-10316B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\641.leela_s-800B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\644.nab_s-5853B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\649.fotonik3d_s-1176B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\654.roms_s-842B.champsimtrace.xz_DRAM_addr.csv",
    "..\\memory_traces_50000\\657.xz_s-3167B.champsimtrace.xz_DRAM_addr.csv"};

MemoryRequestType *read_csv_to_get_memory_trace(FilePathType *_filepath, BenchmarkType *_benchmark)
{
    // Read one .csv file
    FILE *fd1 = fopen(*_filepath, "r");
    if (fd1 == NULL)
    {
        printf("(File_operation.c) Error: Open file .csv failed.\n");
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    uint64_t length = ftell(fd1);
    printf("(File_operation.c) Open file .csv successfully. length = %lld B or %lld KB, ", length, length / 1024);

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

    // count all memory traces
    fseek(fd1, 0, SEEK_SET); // Go to the beginning of this file
    uint64_t count = 0;
    while (1)
    {
        if (getc(fd1) == ',')
            count++;

        if (feof(fd1))
            break;
    }
    printf("count is %lld, ", count);
    _benchmark->length = count;

    // prepare space to load all memory traces
    MemoryRequestType *_MemoryRequest = (MemoryRequestType *)malloc(sizeof(MemoryRequestType) * count);
    if (_MemoryRequest == NULL)
    {
        printf("(File_operation.c) Error: Memory allocation failed.\n");
        exit(1);
    }

    // decode the file and load all memory traces
    uint64_t _max_address = 0;
    uint64_t _min_address = 0;
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
            else // arrive ','
            {
                string1[_count] = '\0';
                break;
            }
        }

        _MemoryRequest[i].address = atoll(string1);
        fread(&_temp, sizeof(char), 1, fd1);
        _MemoryRequest[i].type = _temp - 48; // '0' - 48 = 0 and '1' - 48 = 1
        fread(&_temp, sizeof(char), 1, fd1); // ignore '\n'

        if (i == 0)
        {
            _min_address = _MemoryRequest[i].address;
            _max_address = _min_address;
        }

        if (_MemoryRequest[i].address < _min_address)
            _min_address = _MemoryRequest[i].address;

        if (_MemoryRequest[i].address > _max_address)
            _max_address = _MemoryRequest[i].address;
    }

    printf("max_address: %lld min_address: %lld\n", _max_address, _min_address);
    // print all memory traces
    // for (uint64_t i = 0; i < count; i++)
    // {
    //     printf("%lld %lld, %d\n", i, _MemoryRequest[i].address, _MemoryRequest[i].type);
    // }

    fclose(fd1);
    _benchmark->memorytrace = _MemoryRequest;
    return _MemoryRequest;
}
