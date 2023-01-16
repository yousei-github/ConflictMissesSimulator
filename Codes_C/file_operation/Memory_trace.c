#include "Memory_trace.h"
#include "Configuration.h"
#include <string.h>

#define INTERVAL_FOR_PRINT (10000) // unit: cycle

static int cmp_uint64_t(const void* a, const void* b);

void collect_unique_address_atpagegranularity(MemoryRequestType* _memoryrequest, uint64_t _length, BufferType64bit* _buffer, TraceType _trace)
{
    uint64_t* buffer = (uint64_t*)malloc(sizeof(uint64_t) * _length);
    uint64_t bufferlength = 0;
    if (buffer == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    printf("(%s) buffer's _length is %lld. Start collecting unique address...\n", __func__, _length);

    // get first unique address
    switch (_trace)
    {
    case cache_line:
        buffer[bufferlength++] = _memoryrequest[0].address >> LOG2_PAGE_SIZE_AT_CACHE_LINE;
        break;
    case page:
        buffer[bufferlength++] = _memoryrequest[0].address;
        break;
    default:
        exit(1);
        break;
    }
    for (uint64_t i = 1; i < _length; i++) // go through each memory request
    {
        uint8_t repeat = 0;
        uint64_t get_address = 0;
        switch (_trace)
        {
        case cache_line:
            get_address = _memoryrequest[i].address >> LOG2_PAGE_SIZE_AT_CACHE_LINE;
            break;
        case page:
            get_address = _memoryrequest[i].address;
            break;
        default:
            exit(1);
            break;
        }

        int64_t j = 0;
#if USE_OPENMP == ENABLE
#pragma omp parallel
        {
#pragma omp for
#endif
            for (j = 0; j < bufferlength; j++) // go through the available buffer's members
            {
                if (buffer[j] == get_address)
                {
                    repeat = 1;
                    break;
                }
                if (repeat == 1) // used for other threads that if repeat is already set
                {
                    break;
                }
            }
#if USE_OPENMP == ENABLE
        }
#endif

        if (repeat == 0) // not repeat
        {
            buffer[bufferlength++] = get_address;
            // printf("(%s) %lld buffer: %lld\n", __func__, i, get_address);
        }

#if OUTPUT_DETAIL == ENABLE
        if (i % INTERVAL_FOR_PRINT == 0)
        {
            printf("(%s) memory address: %lld, total memory address: %lld.\n", __func__, i, _length);
        }
#endif
    }
    printf("(%s) Finish collecting unique address.\n", __func__);

    // sorting the memory requests
    printf("(%s) Start sorting the memory requests...\n", __func__);
    qsort(buffer, bufferlength, sizeof(uint64_t), cmp_uint64_t);
    printf("(%s) Finish sorting the memory requests.\n", __func__);

    // for (uint64_t i = 0; i < bufferlength; i++)
    // {
    //     printf("%lld %lld\n", i, buffer[i]);
    // }

#if OUTPUT_DETAIL == ENABLE
    printf("(%s) bufferlength: %lld (# of unique addresses).\n", __func__, bufferlength);
#endif

    // allocate the exact size of buffer for unique memory request storage
    uint64_t* buffer2 = (uint64_t*)malloc(sizeof(uint64_t) * bufferlength);
    if (buffer2 == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }
    memcpy(buffer2, buffer, sizeof(uint64_t) * bufferlength);
    free(buffer);

    _buffer->buffer = buffer2;      // get the unique memory request buffer
    _buffer->length = bufferlength; // store the length of unique memory request buffer
}

uint64_t calculate_memory_footprint(BenchmarkType* _benchmark)
{
#if READ_UNIQUE_ADDRESS_FILE == ENABLE
    read_binary_to_get_unique_memory_trace(_benchmark);
#else
    BufferType64bit buffer1; // store the unique memory requests sorted at ascending order
    collect_unique_address_atpagegranularity(_benchmark->memorytrace, _benchmark->tracelength, &buffer1, _benchmark->trace_type);

    // figure out how many pages does this benchmark need
    uint64_t page_count = buffer1.length;

#if OUTPUT_DETAIL == ENABLE
    printf("page_count: %lld, total used memory: %lld B (%lld KB)\n", page_count, page_count * PAGE_SIZE, page_count * PAGE_SIZE / KB);
#endif

    // allocate the exact size of buffer for page storage
    TotalPageType* page_temp = (TotalPageType*)malloc(sizeof(TotalPageType) * page_count);
    if (page_temp == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    for (uint64_t page_number = 0; page_number < page_count; page_number++)
    {
        page_temp[page_number].base = buffer1.buffer[page_number]; // record this page's base address
    }

    _benchmark->total_page = page_temp;         // load the pointer to all page needed
    _benchmark->total_page_number = page_count; // record the number of used pages
#endif

#if CREATE_UNIQUE_ADDRESS_FILE == ENABLE
    create_binary_to_store_unique_memory_address(_benchmark);
#endif

    //  for (uint64_t i = 0; i < page_count; i++)
    //  {
    //      printf("(%s) page_temp[%lld]: %lld\n", __func__, i, page_temp[i].base);
    //  }

#if READ_UNIQUE_ADDRESS_FILE == DISABLE
    free(buffer1.buffer);
#endif

    return 0;
}

// Compare funtion for sorting the memory requests
int cmp_uint64_t(const void* a, const void* b)
{
    int result = 0;
    if (*(uint64_t*)a > *(uint64_t*)b)
    {
        result = 1;
    }
    else
        result = -1;

    return result;
}