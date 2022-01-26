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

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit != fast)
            {
                uint64_t set_number = current_page_number % _memorystructure->set_size;
                uint64_t temp = ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number;

                ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number = current_page_number;
                _memorystructure->pagemetadata[temp].fast_bit = slow;
                _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
            }
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
        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) Direct_Mapped fast_access: %lld, slow_access: %lld, hit_rate: %f\n", fast_access, slow_access, hit_rate);
}

void hybrid_set_associative_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    uint64_t temp3 = 0;
    for (uint64_t k = 0; k < _benchmark->length2; k++)
    {
        if (_memorystructure->pagemetadata[k].fast_bit == fast)
            temp3++;
    }
    if (temp3 != 0)
    {
        printf("eRROR temp3 %lld\n", temp3);
        //exit(1);
        // clear value
        for (uint64_t i = 0; i < _benchmark->length2; i++)
        {
            _memorystructure->pagemetadata[i].counter = 0;
            _memorystructure->pagemetadata[i].hot_bit = cold;
            _memorystructure->pagemetadata[i].fast_bit = slow;
        }
    }
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

    uint64_t temp2 = 0;
    for (uint64_t k = 0; k < _benchmark->length2; k++)
    {
        if (_memorystructure->pagemetadata[k].fast_bit == fast)
            temp2++;
    }
    if (set_associative != Fully_Associative)
    {
        if (temp2 != _memorystructure->set_size * _memorystructure->set_associative)
        {
            printf("1 temp2 %lld, set_size %lld set_associative, %lld, length2 %lld\n", temp2, _memorystructure->set_size, _memorystructure->set_associative, _benchmark->length2);
            exit(1);
        }
    }
    else
    {
        if (temp2 != _memorystructure->set_size)
        {
            printf("1 temp2 %lld, set_size %lld set_associative, %lld, length2 %lld\n", temp2, _memorystructure->set_size, _memorystructure->set_associative, _benchmark->length2);
            exit(1);
        }
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
        uint64_t set_number = current_page_number % _memorystructure->set_size;
        if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
        {
            fast_access++;
            switch (set_associative)
            {
            case Two_Way:
                if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0] == current_page_number)
                {
                    ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_one;
                }
                else if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1] == current_page_number)
                {
                    ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_two;
                }
                else
                {
                    printf("Error: swap1 error %lld %lld %lld\n", ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0], ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1], current_page_number);
                    // 652 751 850
                    printf(" %lld, %d, %d, %d\n", i, _memorystructure->pagemetadata[652].fast_bit, _memorystructure->pagemetadata[751].fast_bit, _memorystructure->pagemetadata[850].fast_bit); //35155
                    uint64_t temp2 = 0;
                    for (uint64_t k = 0; k < _benchmark->length2; k++)
                    {
                        if (_memorystructure->pagemetadata[k].fast_bit == fast)
                            temp2++;
                    }
                    printf("2 temp2 %lld, set_size %lld, set_number %lld\n", temp2, _memorystructure->set_size, set_number);
                    exit(1);
                }
                break;
            case Four_Way:
                uint8_t flag = 0;
                for (uint8_t j = 0; j < Four_Way; j++)
                {

                    if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == current_page_number)
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_one)
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;
                        }
                        flag = 1;
                    }
                    else
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_four)
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                }
                if (flag == 0)
                {
                    printf("error at LRU\n");
                }
                break;
            case Eight_Way:
                for (uint8_t j = 0; j < Eight_Way; j++)
                {
                    if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == current_page_number)
                    {
                        if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_one)
                        {
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;
                        }
                    }
                    else
                    {
                        if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_eight)
                        {
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                }
                break;
            case Fully_Associative:
                for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                {
                    uint64_t max_position = _memorystructure->set_size - 1;
                    if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN == current_page_number)
                    {
                        if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track != position_one)
                        {
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track -= 1;
                        }
                    }
                    else
                    {
                        if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track != max_position)
                        {
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track += 1;
                        }
                    }
                }
                break;
            default:
                exit(1);
                break;
            }
        }
        else
        {
            slow_access++;
        }

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit != fast)
            {
                uint64_t temp;
                uint64_t old_track = position_one;
                uint64_t old_page_number = 0;
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
                        printf("(Hybrid_simulation.c) Error: swap2 error\n");
                        exit(1);
                    }
                    break;
                case Four_Way:
                    old_track = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[0];
                    old_page_number = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[0];
                    for (uint8_t j = 1; j < Four_Way; j++)
                    {
                        if (old_track < ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j])
                        {
                            old_page_number = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        }
                    }
                    for (uint8_t j = 0; j < Four_Way; j++)
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == old_page_number)
                        {
                            temp = old_page_number;
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                        }
                        else if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] < position_four)
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                    break;
                case Eight_Way:
                    old_track = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[0];
                    old_page_number = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[0];
                    for (uint8_t j = 1; j < Eight_Way; j++)
                    {
                        if (old_track < ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j])
                        {
                            old_page_number = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        }
                    }
                    for (uint8_t j = 0; j < Eight_Way; j++)
                    {
                        if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == old_page_number)
                        {
                            temp = old_page_number;
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                        }
                        else if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] < position_eight)
                        {
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
                    break;
                case Fully_Associative:
                    uint64_t max_position = _memorystructure->set_size - 1;
                    old_track = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[0].track;
                    old_page_number = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[0].PPN;
                    for (uint64_t j = 1; j < _memorystructure->set_size; j++)
                    {
                        if (old_track < ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track)
                        {
                            old_page_number = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                        }
                    }
                    for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                    {
                        if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN == old_page_number)
                        {
                            temp = old_page_number;
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track = position_one;
                        }
                        else if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track < max_position)
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

                temp2 = 0;
                for (uint64_t k = 0; k < _benchmark->length2; k++)
                {
                    if (_memorystructure->pagemetadata[k].fast_bit == fast)
                        temp2++;
                }
                if (set_associative != Fully_Associative)
                {
                    if (temp2 != _memorystructure->set_size * _memorystructure->set_associative)
                    {
                        printf("4 temp2 %lld, set_size %lld set_associative, %lld, length2 %lld\n", temp2, _memorystructure->set_size, _memorystructure->set_associative, _benchmark->length2);
                        exit(1);
                    }
                }
                else
                {
                    if (temp2 != _memorystructure->set_size)
                    {
                        printf("4 temp2 %lld, set_size %lld set_associative, %lld, length2 %lld\n", temp2, _memorystructure->set_size, _memorystructure->set_associative, _benchmark->length2);
                        exit(1);
                    }
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->length2; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    if (_memorystructure->pagemetadata[j].hot_bit != cold)
                    {
                        _memorystructure->pagemetadata[j].hot_bit = cold; // mark cold page

                        if (_memorystructure->pagemetadata[j].fast_bit == fast)
                        {
                            uint64_t set_number2 = j % _memorystructure->set_size;
                            switch (set_associative)
                            {
                            case Two_Way:
                                for (uint8_t k = 0; k < Two_Way; k++)
                                {
                                    if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number2].page_number[k] == j)
                                    {
                                        if (k == 0)
                                            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number2].old_bit = position_two;
                                        else if (k == 1)
                                        {
                                            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number2].old_bit = position_one;
                                        }
                                        else
                                        {
                                            printf("ERROR in detecting cold page\n");
                                            exit(1);
                                        }
                                    }
                                }
                                break;
                            case Four_Way:
                                for (uint8_t k = 0; k < Four_Way; k++)
                                {
                                    if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number2].page_number[k] == j)
                                    {
                                        ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number2].track[k] = position_four;
                                    }
                                }
                                break;
                            case Eight_Way:
                                for (uint8_t k = 0; k < Eight_Way; k++)
                                {
                                    if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number2].page_number[k] == j)
                                    {
                                        ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number2].track[k] = position_eight;
                                    }
                                }
                                break;
                            case Fully_Associative:
                                for (uint64_t k = 0; k < _memorystructure->set_size; k++)
                                {
                                    uint64_t max_position = _memorystructure->set_size - 1;
                                    if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].PPN == j)
                                    {
                                        ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].track = max_position;
                                    }
                                }
                                break;
                            default:
                                exit(1);
                                break;
                            }
                        }
                    }
                }
            }
            // for (uint64_t i = 0; i < _memorystructure->set_size; i++)
            // {
            //     printf("%lld ", ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number);
            // }
            // printf("\n");
        }
        cycle++;
    }

    temp2 = 0;
    for (uint64_t k = 0; k < _benchmark->length2; k++)
    {
        if (_memorystructure->pagemetadata[k].fast_bit == fast)
            temp2++;
    }
    //printf("Final check temp2 %lld, set_size %lld\n", temp2, _memorystructure->set_size);

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f\n", set_associative, fast_access, slow_access, hit_rate);
}
