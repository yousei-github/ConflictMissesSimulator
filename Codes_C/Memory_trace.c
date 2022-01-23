#include "Memory_trace.h"
#include <stdlib.h>
#include <string.h>
#include "DataType.h"

static int cmp_uint64_t(const void *a, const void *b);

void collect_unique_address(MemoryRequestType *_memoryrequest, uint64_t _length, BufferType64bit *_buffer)
{
    uint64_t *buffer = (uint64_t *)malloc(sizeof(uint64_t) * _length);
    uint64_t bufferlength = 0;
    if (buffer == NULL)
    {
        printf("(Memory_trace.c) Error: Memory allocation failed.\n");
        exit(1);
    }

    buffer[bufferlength++] = _memoryrequest[0].address; // get first unique address
    for (uint64_t i = 1; i < _length; i++)              // go through each memory request
    {
        uint8_t repeat = 0;
        for (uint64_t j = 0; j < bufferlength; j++) // go through the available buffer's members
        {
            if (buffer[j] == _memoryrequest[i].address)
            {
                repeat = 1;
                break;
            }
        }
        if (repeat == 0) // not repeat
        {
            buffer[bufferlength++] = _memoryrequest[i].address;
            //printf("(Memory_trace.c) %d buffer: %lld\n", i, _memoryrequest[i].address);
        }
    }

    // sorting the memory requests
    qsort(buffer, bufferlength, sizeof(uint64_t), cmp_uint64_t);

    // for (uint64_t i = 0; i < bufferlength; i++)
    // {
    //     printf("%lld %lld\n", i, buffer[i]);
    // }
    printf("(Memory_trace.c) bufferlength: %lld\n", bufferlength);

    uint64_t *buffer2 = (uint64_t *)malloc(sizeof(uint64_t) * bufferlength);
    memcpy(buffer2, buffer, sizeof(uint64_t) * bufferlength);
    free(buffer);

    _buffer->buffer = buffer2;
    _buffer->length = bufferlength;
}

uint64_t calculate_memory_footprint(BenchmarkType *_benchmark)
{
    BufferType64bit buffer1;
    collect_unique_address(_benchmark->memorytrace, _benchmark->length, &buffer1);

    uint64_t page_count = 1;
    uint64_t base = buffer1.buffer[0];
    for (uint64_t i = 0; i < buffer1.length; i++)
    {
        if (buffer1.buffer[i] < base + PAGE_SIZE)
        {
            ;
        }
        else
        {
            printf("(Memory_trace.c) base: %lld at %lld\n", base, page_count);
            page_count++;             // new page is needed
            base = buffer1.buffer[i]; // update the base
        }
    }
    printf("(Memory_trace.c) base: %lld at %lld\n", base, page_count);
    printf("(Memory_trace.c) page_count: %lld\n", page_count);

    free(buffer1.buffer);

    return 0;
}

// Compare funtion for sorting the memory requests
int cmp_uint64_t(const void *a, const void *b)
{
    int result = 0;
    if (*(uint64_t *)a > *(uint64_t *)b)
    {
        result = 1;
    }
    else
        result = -1;

    return result;
}