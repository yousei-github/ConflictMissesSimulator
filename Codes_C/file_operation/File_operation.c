#include "File_operation.h"
#include "Configuration.h"
#include "string.h"

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

static void find_file_name(const char* _filepath, BenchmarkType* _benchmark);

MemoryRequestType* read_csv_to_get_memory_trace(FilePathType* _filepath, BenchmarkType* _benchmark)
{
    // Read one .csv file
    FILE* fd1 = fopen(*_filepath, "r");
    if (fd1 == NULL)
    {
        printf("(File_operation.c) Error: Open file .csv failed.\n");
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    uint64_t length = ftell(fd1);

#if OUTPUT_DETAIL == ENABLE
    printf("(File_operation.c) Open file .csv successfully. length = %lld B or %lld KB, ", length, length / 1024);
#endif

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
#if OUTPUT_DETAIL == ENABLE
    printf("count is %lld, ", count);
#endif
    _benchmark->tracelength = count;

    // prepare space to load all memory traces
    MemoryRequestType* _MemoryRequest = (MemoryRequestType*)malloc(sizeof(MemoryRequestType) * count);
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
#if OUTPUT_DETAIL == ENABLE
    printf("max_address: %lld min_address: %lld\n", _max_address, _min_address);
#endif
    // print all memory traces
    // for (uint64_t i = 0; i < count; i++)
    // {
    //     printf("%lld %lld, %d\n", i, _MemoryRequest[i].address, _MemoryRequest[i].type);
    // }

    fclose(fd1);
    _benchmark->memorytrace = _MemoryRequest;
    _benchmark->hit_rate = 0;
    _benchmark->conflict_miss_rate = 0;
    return _MemoryRequest;
}

// e.g., _filepath = .\memory_traces_50000\600.perlbench_s-210B.champsimtrace_64B
/** @trace_structure:
 * struct
 * {
 *  uint64_t address;
 *  uint8_t type; // Read: 0, Write: 1
 * };
 */
MemoryRequestType* read_binary_to_get_memory_trace(const char* _filepath, BenchmarkType* _benchmark, TraceType _trace)
{
    // Read one binary file
    FILE* fd1 = fopen(_filepath, "rb");
    if (fd1 == NULL)
    {
        printf("(%s) Error: Open binary file failed.\n", __func__);
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    uint64_t length = ftell(fd1);

#if OUTPUT_DETAIL == ENABLE
    printf("(%s) Open binary file successfully. length = %lld B or %lld KB.\n", __func__, length, length / 1024);
#endif

    find_file_name(_filepath, _benchmark);

    // count all memory traces
    uint64_t count = 0;
    uint8_t memoryrequest_size = (sizeof(uint64_t) + sizeof(uint8_t));

    if ((length % memoryrequest_size) == 0)
    {
        count = length / memoryrequest_size;
    }
    else
    {
        printf("(%s) The number of memory request is not correct.\n", __func__);
        exit(1);
    }

#if OUTPUT_DETAIL == ENABLE
    printf("(%s) count is %lld, memoryrequest_size is %d.\n", __func__, count, memoryrequest_size);
#endif
    _benchmark->tracelength = count; // record the total number of memory request or the length of memory trace

    // prepare space to load all memory traces
    MemoryRequestType* _MemoryRequest = (MemoryRequestType*)malloc(sizeof(MemoryRequestType) * count);
    if (_MemoryRequest == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    // decode the file and load all memory traces
    printf("(%s) Decoding the file and loading all memory traces...\n", __func__);
    uint64_t _max_address = 0;
    uint64_t _min_address = 0;
    uint8_t address_size = sizeof(uint64_t);
    uint8_t type_size = sizeof(uint8_t);
    fseek(fd1, 0, SEEK_SET); // Go to the beginning of this file
    for (uint64_t i = 0; i < count; i++)
    {
        uint8_t read_size = 0;
        read_size = fread(&_MemoryRequest[i].address, address_size, 1, fd1);
        if (read_size != 1)
        {
            printf("(%s) Error: file read failed at address_size(%d) at %lld. read_size is %d.\n", __func__, address_size, i, read_size);
            exit(1);
        }

        read_size = fread(&_MemoryRequest[i].type, type_size, 1, fd1);
        if (read_size != 1)
        {
            printf("(%s) Error: file read failed at type_size(%d) at %lld. read_size is %d.\n", __func__, type_size, i, read_size);
            exit(1);
        }

        if ((_MemoryRequest[i].type == 0) || (_MemoryRequest[i].type == 1)) // check the correctness of memory request
        {
            ; // nothing to do here
        }
        else
        {
            printf("(%s) Error: Memory request read failed.\n", __func__);
            exit(1);
        }

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

    // check whether reach the end of file
    uint8_t temp = 0;
    fread(&temp, 1, 1, fd1);
    if (feof(fd1))
    {
        printf("(%s) Reach end of file. Read %lld traces.\n", __func__, count);
    }
    else
    {
        printf("(%s) Error: Doesn't reach end of file.\n", __func__);
        exit(1);
    }

    printf("(%s) Finish decoding the file and loading all memory traces.\n", __func__);

    _benchmark->trace_type = _trace; // load the trace type
    switch (_benchmark->trace_type)
    {
    case cache_line:
        _benchmark->address_space = _max_address * CACHE_LINE;
        break;
    case page:
        _benchmark->address_space = _max_address * PAGE_SIZE;
        break;
    default:
        exit(1);
        break;
    }
#if OUTPUT_DETAIL == ENABLE
    printf("max_address: %lld, min_address: %lld. The address space is %lld B (%lld GB).\n", _max_address, _min_address, _benchmark->address_space, _benchmark->address_space / KB / KB / KB);
#endif
    // print all memory traces
    // for (uint64_t i = 0; i < count; i++)
    // {
    //     printf("%lld %lld, %d\n", i, _MemoryRequest[i].address, _MemoryRequest[i].type);
    // }

    fclose(fd1);
    _benchmark->memorytrace = _MemoryRequest;            // load the pointer to memory trace
    _benchmark->hit_rate = 0;                            // initialize hit_rate
    _benchmark->conflict_miss_rate = 0;                  // initialize conflict_miss_rate
    _benchmark->migration_data_NM_to_FM_in_byte = 0;     // initialize migration_data_NM_to_FM_in_byte
    _benchmark->unuse_migrated_cache_line_NM_number = 0; // initialize unuse_migrated_cache_line_NM_number
    return _MemoryRequest;
}

void read_binary_to_get_unique_memory_trace(BenchmarkType* _benchmark)
{
    // read the sorted unique memory requests at page granularity
    char file_name[FILE_NAME_LENGTH];
    strcpy(file_name, _benchmark->file_name);
    strcat(file_name, "_unique");
    printf("(%s) file_name is %s.\n", __func__, file_name);

    FILE* fd1 = fopen(file_name, "rb");
    if (fd1 == NULL)
    {
        printf("(%s) Error: Open binary file (%s) failed.\n", __func__, file_name);
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    uint64_t length = ftell(fd1);

    // count all unique pages
    uint64_t page_count = 0;
    uint8_t memoryrequest_size = (sizeof(uint64_t));

    if ((length % memoryrequest_size) == 0)
    {
        page_count = length / memoryrequest_size;
    }
    else
    {
        printf("(%s) The number of memory request is not correct.\n", __func__);
        exit(1);
    }

#if OUTPUT_DETAIL == ENABLE
    printf("(%s) page_count is %lld, memoryrequest_size is %d.\n", __func__, page_count, memoryrequest_size);
#endif

    // prepare space for page storage
    TotalPageType* page_temp = (TotalPageType*)malloc(sizeof(TotalPageType) * page_count);
    if (page_temp == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    fseek(fd1, 0, SEEK_SET); // Go to the beginning of this file
    for (uint64_t page_number = 0; page_number < page_count; page_number++)
    {
        uint8_t read_size = 0;
        read_size = fread(&page_temp[page_number].base, memoryrequest_size, 1, fd1);
        if (read_size != 1)
        {
            printf("(%s) Error: file read failed at size(%d) at %lld. read_size is %d.\n", __func__, memoryrequest_size, page_number, read_size);
            exit(1);
        }
    }

    // check whether reach the end of file
    uint8_t temp = 0;
    fread(&temp, 1, 1, fd1);
    if (feof(fd1))
    {
        printf("(%s) Reach end of file. Read %lld traces.\n", __func__, page_count);
    }
    else
    {
        printf("(%s) Error: Doesn't reach end of file.\n", __func__);
        exit(1);
    }

    fclose(fd1);

    printf("(%s) Finish file read.\n", __func__);

    _benchmark->total_page = page_temp;         // load the pointer to all page needed
    _benchmark->total_page_number = page_count; // record the number of used pages
}

void read_binary_to_get_segment_address_for_memory_trace(BenchmarkType* _benchmark)
{
    // read the sorted unique memory requests at page granularity
    char file_name[FILE_NAME_LENGTH];
    strcpy(file_name, _benchmark->file_name);
    strcat(file_name, "_with_page_number");
    printf("(%s) file_name is %s.\n", __func__, file_name);

    FILE* fd1 = fopen(file_name, "rb");
    if (fd1 == NULL)
    {
        printf("(%s) Error: Open binary file (%s) failed.\n", __func__, file_name);
        exit(1);
    }
    fseek(fd1, 0, SEEK_END); // Go to the end of this file
    uint64_t length = ftell(fd1);

    // count all memory traces
    uint64_t count = 0;
    uint8_t memoryrequest_size = (sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint8_t));

    if ((length % memoryrequest_size) == 0)
    {
        count = length / memoryrequest_size;
    }
    else
    {
        printf("(%s) The number of memory request is not correct.\n", __func__);
        exit(1);
    }

#if OUTPUT_DETAIL == ENABLE
    printf("(%s) count is %lld, memoryrequest_size is %d.\n", __func__, count, memoryrequest_size);
#endif

    if (count != _benchmark->tracelength)
    {
        printf("(%s) The number of memory request is not correct. It should be %lld.\n", __func__, _benchmark->tracelength);
        exit(1);
    }

    uint8_t address_size = sizeof(uint64_t);
    uint8_t segment_address_size = sizeof(uint64_t);
    uint8_t type_size = sizeof(uint8_t);
    fseek(fd1, sizeof(uint64_t), SEEK_SET); // Go to the beginning of this file with offset
    // assign each memory request its page number
    printf("(%s) Start assigning each memory request its page number...\n", __func__);
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through each memory request
    {
        uint8_t read_size = 0;
        read_size = fread(&_benchmark->memorytrace[i].segment_address, segment_address_size, 1, fd1);
        if (read_size != 1)
        {
            printf("(%s) Error: file read failed at size(%d) at %lld. read_size is %d.\n", __func__, memoryrequest_size, i, read_size);
            exit(1);
        }

        if (i == _benchmark->tracelength - 1)
        {
            fseek(fd1, type_size, SEEK_CUR);
        }
        else
        {
            fseek(fd1, type_size + address_size, SEEK_CUR);
        }

        // printf("%lld %lld, %lld, %d\n", i, _benchmark->memorytrace[i].address, _benchmark->memorytrace[i].segment_address, _benchmark->memorytrace[i].type);
    }
    printf("(%s) End assigning each memory request its page number.\n", __func__);

    // check whether reach the end of file
    uint8_t temp = 0;
    fread(&temp, 1, 1, fd1);
    if (feof(fd1))
    {
        printf("(%s) Reach end of file. Read %lld traces.\n", __func__, count);
    }
    else
    {
        printf("(%s) Error: Doesn't reach end of file.\n", __func__);
        exit(1);
    }

    fclose(fd1);

    printf("(%s) Finish file read.\n", __func__);
}

void create_binary_to_store_unique_memory_address(BenchmarkType* _benchmark)
{
    // store the sorted unique memory requests at page granularity
    char file_name[FILE_NAME_LENGTH];
    strcpy(file_name, _benchmark->file_name);
    strcat(file_name, "_unique");
    printf("(%s) file_name is %s.\n", __func__, file_name);

    FILE* fd1 = fopen(file_name, "wb");
    if (fd1 == NULL)
    {
        printf("(%s) Error: Open binary file failed.\n", __func__);
        exit(1);
    }
    for (uint64_t i = 0; i < _benchmark->total_page_number; i++)
    {
        fwrite(&(_benchmark->total_page[i].base), sizeof(uint64_t), 1, fd1);
    }
    fclose(fd1);

    printf("(%s) Finish file write.\n", __func__);
}

void create_binary_to_store_memory_trace_with_segment_address(BenchmarkType* _benchmark)
{
    // store each memory request with its page number
    char file_name[FILE_NAME_LENGTH];
    strcpy(file_name, _benchmark->file_name);
    strcat(file_name, "_with_page_number");
    printf("(%s) file_name is %s.\n", __func__, file_name);

    FILE* fd1 = fopen(file_name, "wb");
    if (fd1 == NULL)
    {
        printf("(%s) Error: Open binary file failed.\n", __func__);
        exit(1);
    }
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through each memory request _benchmark->memorytrace[i].address
    {
        fwrite(&(_benchmark->memorytrace[i].address), sizeof(uint64_t), 1, fd1);
        fwrite(&(_benchmark->memorytrace[i].segment_address), sizeof(uint64_t), 1, fd1);
        fwrite(&(_benchmark->memorytrace[i].type), sizeof(uint8_t), 1, fd1);
    }
    fclose(fd1);

    printf("(%s) Finish file write.\n", __func__);
}

void find_file_name(const char* _filepath, BenchmarkType* _benchmark)
{
    // store file's name
    char file_name[FILE_NAME_LENGTH];
    strcpy(file_name, _filepath);
    printf("(%s) file_name = %s.\n", __func__, file_name);

    char* token[DIRECTORY_NUMBER];
    uint8_t token_count = 0;
    const char* delimiters = "/\\";
    /* get the first token */
    token[token_count] = strtok(file_name, delimiters);
    /* walk through other tokens */
    while (token[token_count] != NULL)
    {
        // printf(" %s\n", token[token_count]);
        token[++token_count] = strtok(NULL, delimiters);
    }
    strcpy(_benchmark->file_name, token[token_count - 1]);
    printf("(%s) file_name is %s.\n", __func__, _benchmark->file_name);
}
