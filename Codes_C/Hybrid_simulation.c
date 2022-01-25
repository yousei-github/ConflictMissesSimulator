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

void hybrid_set_associative_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;
    // fill page numbers into the fast memory
    switch (set_associative)
    {
    case Two_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[0] = i;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[1] = i + _memorystructure->set_size;
            _memorystructure->pagemetadata[i].fast_bit = fast;
            _memorystructure->pagemetadata[i + _memorystructure->set_size].fast_bit = fast;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].old_bit = position_one;
        }
        break;
    case Four_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            for (uint64_t j = 0; j < Four_Way; j++) // go through the ways
            {
                ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number[j] = i + _memorystructure->set_size * j;
                _memorystructure->pagemetadata[i + _memorystructure->set_size * j].fast_bit = fast;
                ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].track[j] = position_one + j;
            }
        }
        break;
    case Eight_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            for (uint64_t j = 0; j < Eight_Way; j++) // go through the ways
            {
                ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number[j] = i + _memorystructure->set_size * j;
                _memorystructure->pagemetadata[i + _memorystructure->set_size * j].fast_bit = fast;
                ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track[j] = position_one + j;
            }
        }
        break;
    case Fully_Associative:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].PPN = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].track = position_one + i;
        }
        break;
    default:
        exit(1);
        break;
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
                uint64_t temp;
                switch (set_associative)
                {
                case Two_Way:
                    if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit == position_one)
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1];
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1] = current_page_number;
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_two;
                    }
                    else if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit == position_two)
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0];
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0] = current_page_number;
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_one;
                    }
                    else
                    {
                        printf("(Hybrid_simulation.c) Error: swap error\n");
                        exit(1);
                    }
                    break;
                case Four_Way:
                    for (uint8_t j = 0; j < Four_Way; j++)
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] == position_four)
                        {
                            temp = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                        }
                        else
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                    break;
                case Eight_Way:
                    for (uint8_t j = 0; j < Eight_Way; j++)
                    {
                        if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] == position_eight)
                        {
                            temp = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                        }
                        else
                        {
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                    break;
                case Fully_Associative:
                    for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                    {
                        uint64_t max_position = _memorystructure->set_size - 1;
                        if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track == max_position)
                        {
                            temp = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track = position_one;
                        }
                        else
                        {
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track += 1;
                        }
                    }
                    break;
                default:
                    exit(1);
                    break;
                }
                _memorystructure->pagemetadata[temp].fast_bit = slow;
                _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
            }
        }

        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) fast_access: %lld, slow_access: %lld, hit_rate: %f\n", fast_access, slow_access, hit_rate);
}