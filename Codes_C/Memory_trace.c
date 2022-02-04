#include "Memory_trace.h"
#include "Configuration.h"
#include <string.h>

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

#if OUTPUT_DETAIL == ENABLE
    printf("(Memory_trace.c) bufferlength: %lld (# of unique addresses), ", bufferlength);
#endif

    // allocate the exact size of buffer for unique memory request storage
    uint64_t *buffer2 = (uint64_t *)malloc(sizeof(uint64_t) * bufferlength);
    if (buffer2 == NULL)
    {
        printf("(Memory_trace.c) Error: Memory allocation failed.\n");
        exit(1);
    }
    memcpy(buffer2, buffer, sizeof(uint64_t) * bufferlength);
    free(buffer);

    _buffer->buffer = buffer2;
    _buffer->length = bufferlength;
}

uint64_t calculate_memory_footprint(BenchmarkType *_benchmark, TraceType _trace)
{
    BufferType64bit buffer1; // store the unique memory requests sorted at ascending order
    collect_unique_address(_benchmark->memorytrace, _benchmark->tracelength, &buffer1);

    uint64_t page_count = 1;
    uint64_t base = buffer1.buffer[0];
    //printf("(Memory_trace.c) base: %lld at %lld\n", base, page_count);
    switch (_trace)
    {
    case byte:
        for (uint64_t i = 1; i < buffer1.length; i++) // figure out how many pages does this benchmark need
        {
            if (buffer1.buffer[i] < base + PAGE_SIZE)
            {
                ; // do nothing
            }
            else
            {
                page_count++;             // new page is needed
                base = buffer1.buffer[i]; // update the base
                //printf("(Memory_trace.c) base: %lld at %lld\n", base, page_count);
            }
        }
        break;
    case page:
        // figure out how many pages does this benchmark need
        page_count = buffer1.length;
        break;
    default:
        exit(1);
        break;
    }

#if OUTPUT_DETAIL == ENABLE
    printf("page_count: %lld, total used memory: %lld B (%lld KB)\n", page_count, page_count * PAGE_SIZE, page_count * PAGE_SIZE / KB);
#endif

    // allocate the exact size of buffer for page storage
    TotalPageType *page_temp = (TotalPageType *)malloc(sizeof(TotalPageType) * page_count);
    if (page_temp == NULL)
    {
        printf("(Memory_trace.c) Error: Memory allocation failed.\n");
        exit(1);
    }
    uint64_t page_number = 0;
    base = buffer1.buffer[0];
    page_temp[page_number].base = base;
    page_temp[page_number].page_number = page_number;
    switch (_trace)
    {
    case byte:
        for (uint64_t i = 1; i < buffer1.length; i++)
        {
            if (buffer1.buffer[i] < base + PAGE_SIZE)
            {
                ; // do nothing
            }
            else
            {
                page_number++;            // new page is needed
                base = buffer1.buffer[i]; // update the base
                page_temp[page_number].base = base;
                page_temp[page_number].page_number = page_number;
            }
        }
        break;
    case page:
        for (uint64_t i = 1; i < buffer1.length; i++)
        {
            page_number++;            // new page is needed
            base = buffer1.buffer[i]; // update the base
            page_temp[page_number].base = base;
            page_temp[page_number].page_number = page_number;
        }
        break;
    default:
        exit(1);
        break;
    }

    _benchmark->totalpage = page_temp;
    _benchmark->totoalpagelength = page_count;
    // for (uint64_t i = 0; i < page_count; i++)
    // {
    //     printf("(Memory_trace.c) page_temp[%lld]: %lld %lld\n", i, page_temp[i].base, page_temp[i].page_number);
    // }

    // assign each memory request its page number
    for (uint64_t i = 0; i < _benchmark->tracelength; i++)
    {
        switch (_trace)
        {
        case byte:
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                if ((_benchmark->totalpage[j].base <= _benchmark->memorytrace[i].address) && (_benchmark->memorytrace[i].address < _benchmark->totalpage[j].base + PAGE_SIZE))
                {
                    _benchmark->memorytrace[i].page_number = _benchmark->totalpage[j].page_number;
                    break;
                }
            }
            break;
        case page:
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                if (_benchmark->totalpage[j].base == _benchmark->memorytrace[i].address)
                {
                    _benchmark->memorytrace[i].page_number = _benchmark->totalpage[j].page_number;
                    break;
                }
            }
            break;
        default:
            exit(1);
            break;
        }

        //printf("%lld %lld, %lld, %d\n", i, _benchmark->memorytrace[i].address, _benchmark->memorytrace[i].page_number, _benchmark->memorytrace[i].type);
    }

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