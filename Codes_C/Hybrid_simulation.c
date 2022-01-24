#include "Hybrid_simulation.h"

#define INTERVAL_FOR_DECREMENT (1000) // unit: cycle

void hybrid_direct_mapped_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    // fill page numbers into the fast memory
    for (uint64_t i = 0; i < _memorystructure->set_size; i++)
    {
        ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number = i;
        _memorystructure->pagemetadata[i].fast_bit = fast;
    }

    // for (uint64_t i = 0; i < _memorystructure->set_size; i++)
    // {
    //     printf("%lld ", ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number);
    // }
    // printf("\n");

    uint64_t fast_access = 0;
    uint64_t slow_access = 0;
    uint64_t cycle = 1;
    for (uint64_t i = 0; i < _benchmark->length; i++) // go through all memory requests
    {
        uint64_t current_page_number = _benchmark->memorytrace[i].page_number;
        if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
        {
            fast_access++;
        }
        else
        {
            slow_access++;
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->length2; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    _memorystructure->pagemetadata[j].hot_bit = cold; // mark cold page
                }
            }
            // for (uint64_t i = 0; i < _memorystructure->set_size; i++)
            // {
            //     printf("%lld ", ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number);
            // }
            // printf("\n");
        }

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
            {
                ;
            }
            else
            {
                uint64_t set_number = current_page_number % _memorystructure->set_size;
                uint64_t temp = ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number;
                ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number = current_page_number;
                _memorystructure->pagemetadata[temp].fast_bit = slow;
                _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
            }
        }

        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) fast_access: %lld, slow_access: %lld, hit_rate: %f\n", fast_access, slow_access, hit_rate);
}
