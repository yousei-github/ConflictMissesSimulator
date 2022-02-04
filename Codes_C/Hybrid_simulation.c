#include "Hybrid_simulation.h"

#define INTERVAL_FOR_DECREMENT (1000) // unit: cycle

/* Declaration */
extern void coldqueue_initialization(uint8_t number);
extern uint64_t coldqueue_size(uint8_t number);
extern uint8_t coldqueue_empty(uint8_t number);
extern void coldqueue_enqueue(uint8_t number, uint64_t element);
extern uint64_t coldqueue_dequeue(uint8_t number);
extern void hotqueue_initialization(uint8_t number);
extern uint64_t hotqueue_size(uint8_t number);
extern uint8_t hotqueue_empty(uint8_t number);
extern void hotqueue_enqueue(uint8_t number, uint64_t element);
extern uint64_t hotqueue_dequeue(uint8_t number);

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
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
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
                swap_count++;
                if (_memorystructure->pagemetadata[temp].hot_bit == hot)
                {
                    swappedhotpage_count++;
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
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
    printf("(Hybrid_simulation.c) Direct_Mapped fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", fast_access, slow_access, hit_rate, swap_count, (float)swappedhotpage_count / swap_count);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / swap_count;
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
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
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
                    printf("Error: swap1 error");
                    exit(1);
                }
                break;
            case Four_Way:
                for (uint8_t j = 0; j < Four_Way; j++)
                {
                    if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == current_page_number)
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_one)
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;
                        }
                    }
                    else
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] != position_four)
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] += 1;
                        }
                    }
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
                swap_count++;
                if (_memorystructure->pagemetadata[temp].hot_bit == hot)
                {
                    swappedhotpage_count++;
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
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

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", set_associative, fast_access, slow_access, hit_rate, swap_count, (float)swappedhotpage_count / swap_count);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / swap_count;
}

void hybrid_fully_associative_simulation_usingqueue(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    if (set_associative != Fully_Associative)
    {
        exit(1);
    }
    // fill page numbers into the fast memory
    for (uint64_t i = 0; i < _memorystructure->set_size; i++)
    {
        ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].PPN = i;
        _memorystructure->pagemetadata[i].fast_bit = fast;
        ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].track = position_one + i;
    }

    uint64_t fast_access = 0;
    uint64_t slow_access = 0;
    uint64_t cycle = 1;
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
    {
        uint64_t current_page_number = _benchmark->memorytrace[i].page_number;
        uint64_t set_number = current_page_number % _memorystructure->set_size;
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
            if (_memorystructure->pagemetadata[current_page_number].fast_bit == slow)
            {
                uint8_t flag1 = 0;
                while (coldqueue_empty(0) == 0) // if the cold queue is not empty
                {
                    uint64_t temp = coldqueue_dequeue(0); // get a member
                    uint64_t old_page_number = 0;
                    //printf("dequeue coldqueue_size %lld temp %lld current_page_number %lld\n", coldqueue_size(0), temp, current_page_number);
                    if ((_memorystructure->pagemetadata[temp].hot_bit == cold) && (_memorystructure->pagemetadata[temp].fast_bit == fast)) // if the member is still cold
                    {
                        uint8_t flag = 0;
                        for (uint64_t j = 0; j < _memorystructure->set_size; j++) // go through the fast memory
                        {
                            if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN == temp)
                            {
                                old_page_number = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                                ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                                _memorystructure->pagemetadata[old_page_number].fast_bit = slow;
                                _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
                                swap_count++;
                                flag = 1;
                                flag1 = 1;
                                break;
                            }
                        }
                        if (flag == 0)
                        {
                            printf("Error at cold, old_page_number %lld", _memorystructure->pagemetadata[old_page_number].fast_bit);
                            exit(1);
                        }
                        //printf("2 coldqueue_size %lld\n", coldqueue_size(0));
                        break;
                    }
                }

                if (flag1 == 0)
                {
                    uint64_t temp = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[0].PPN;
                    uint64_t counter = _memorystructure->pagemetadata[temp].counter;
                    uint64_t vpn = 0;
                    for (uint64_t j = 1; j < _memorystructure->set_size; j++) // go through the fast memory
                    {
                        uint64_t temp2 = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                        if (_memorystructure->pagemetadata[temp2].counter < counter)
                        {
                            temp = temp2;
                            counter = _memorystructure->pagemetadata[temp2].counter;
                            vpn = j;
                        }
                    }
                    //old_page_number = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                    ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[vpn].PPN = current_page_number;
                    _memorystructure->pagemetadata[temp].fast_bit = slow;
                    _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
                    swap_count++;
                    swappedhotpage_count++;
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    if (_memorystructure->pagemetadata[j].hot_bit == hot)
                    {
                        _memorystructure->pagemetadata[j].hot_bit = cold; // mark cold page

                        if (_memorystructure->pagemetadata[j].fast_bit == fast) // new cold pages in fast memory need go to the queue
                        {
                            coldqueue_enqueue(0, j);
                            //printf("enqueue: %lld coldqueue_size %lld\n", j, coldqueue_size(0));
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

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", set_associative, fast_access, slow_access, hit_rate, swap_count, (float)swappedhotpage_count / swap_count);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / swap_count;
}

// fully associative use two queues to cache cold and hot pages
void hybrid_simulation(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    // fill page numbers into the fast memory
    switch (set_associative)
    {
    case Direct_Mapped:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
        }
        break;
    case Two_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[0] = i;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[1] = i + _memorystructure->set_size;
            _memorystructure->pagemetadata[i].fast_bit = fast;
            _memorystructure->pagemetadata[i + _memorystructure->set_size].fast_bit = fast;
        }
        break;
    case Four_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            for (uint64_t j = 0; j < Four_Way; j++) // go through the ways
            {
                ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number[j] = i + _memorystructure->set_size * j;
                _memorystructure->pagemetadata[i + _memorystructure->set_size * j].fast_bit = fast;
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
            }
        }
        break;
    case Fully_Associative:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].PPN = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
        }
        break;
    default:
        exit(1);
        break;
    }

    uint64_t fast_access = 0;
    uint64_t slow_access = 0;
    uint64_t cycle = 1;
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    uint64_t unswappedhotpage_count = 0;
    //uint8_t active_hotqueue = 0;                      // 0 or 1
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
    {
        uint64_t current_page_number = _benchmark->memorytrace[i].page_number;
        uint64_t set_number = current_page_number % _memorystructure->set_size;
        if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
        {
            fast_access++;
            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                unswappedhotpage_count++;
            }
        }
        else
        {
            slow_access++;

            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                swappedhotpage_count++;
            }
        }

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit == slow)
            {
                uint64_t temp;
                uint8_t swap_tag = 0;

                switch (set_associative)
                {
                case Direct_Mapped:
                    temp = ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number;
                    if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                    {
                        swap_tag = 1; // start swapping
                        ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number = current_page_number;
                    }
                    break;
                case Two_Way:
                    for (uint8_t j = 0; j < Two_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Four_Way:
                    for (uint8_t j = 0; j < Four_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Eight_Way:
                    for (uint8_t j = 0; j < Eight_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Fully_Associative:
                    while (coldqueue_empty(0) == 0) // if the cold queue is not empty
                    {
                        temp = coldqueue_dequeue(0);                                                                                           // get a member (page_number)
                        if ((_memorystructure->pagemetadata[temp].hot_bit == cold) && (_memorystructure->pagemetadata[temp].fast_bit == fast)) // if the member is still cold
                        {
                            uint8_t flag = 0;
                            for (uint64_t j = 0; j < _memorystructure->set_size; j++) // go through the fast memory
                            {
                                if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN == temp)
                                {
                                    flag = 1;
                                    swap_tag = 1; // start swapping
                                    ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                                    break;
                                }
                            }
                            if (flag == 0)
                            {
                                printf("Error at cold, old_page_number %lld", _memorystructure->pagemetadata[temp].fast_bit);
                                exit(1);
                            }
                            break;
                        }
                    }
                    break;
                default:
                    exit(1);
                    break;
                }

                if (swap_tag == 1) // continuing swapping
                {
                    _memorystructure->pagemetadata[temp].fast_bit = slow;
                    _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
                    swap_count++;
                }
                else if (set_associative == Fully_Associative) // swapping is unsuccessful, but we need buffer this hot page until new cold page is detected
                {
                    hotqueue_enqueue(0, current_page_number);
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    if (_memorystructure->pagemetadata[j].hot_bit == hot)
                    {
                        _memorystructure->pagemetadata[j].hot_bit = cold; // mark new cold page

                        switch (set_associative)
                        {
                        case Direct_Mapped:
                            break;
                        case Two_Way:
                            break;
                        case Four_Way:
                            break;
                        case Eight_Way:
                            break;
                        case Fully_Associative:
                            if (_memorystructure->pagemetadata[j].fast_bit == fast) // new cold pages in fast memory need go to the queue
                            {
                                uint8_t find_hotpage = 0;
                                while (hotqueue_empty(0) == 0) // if the hot queue is not empty
                                {
                                    uint8_t temp = hotqueue_dequeue(0);                                                                                   // get a member (page_number)
                                    if ((_memorystructure->pagemetadata[temp].hot_bit == hot) && (_memorystructure->pagemetadata[temp].fast_bit == slow)) // if the member is still hot
                                    {
                                        uint8_t flag = 0;
                                        for (uint64_t k = 0; k < _memorystructure->set_size; k++) // go through the fast memory
                                        {
                                            if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].PPN == j)
                                            {
                                                flag = 1;
                                                find_hotpage = 1;
                                                ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].PPN = temp;
                                                _memorystructure->pagemetadata[j].fast_bit = slow;
                                                _memorystructure->pagemetadata[temp].fast_bit = fast;
                                                break;
                                            }
                                        }
                                        if (flag == 0)
                                        {
                                            printf("Error at hot, old_page_number %lld", j);
                                            exit(1);
                                        }
                                        break;
                                    }
                                }

                                if (find_hotpage == 0)
                                    coldqueue_enqueue(0, j);
                                //printf("enqueue: %lld coldqueue_size %lld\n", j, coldqueue_size(0));
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
        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / (unswappedhotpage_count + swappedhotpage_count);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", set_associative, fast_access, slow_access, hit_rate, swap_count, _benchmark->conflictmiss_rate);
}

// if a hot page is detected in slow memory, it will only be migrated into fast memory when its set have cold pages.
void hybrid_simulation2(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    // fill page numbers into the fast memory
    switch (set_associative)
    {
    case Direct_Mapped:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
        }
        break;
    case Two_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[0] = i;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number[1] = i + _memorystructure->set_size;
            _memorystructure->pagemetadata[i].fast_bit = fast;
            _memorystructure->pagemetadata[i + _memorystructure->set_size].fast_bit = fast;
        }
        break;
    case Four_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            for (uint64_t j = 0; j < Four_Way; j++) // go through the ways
            {
                ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number[j] = i + _memorystructure->set_size * j;
                _memorystructure->pagemetadata[i + _memorystructure->set_size * j].fast_bit = fast;
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
            }
        }
        break;
    case Fully_Associative:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].PPN = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
        }
        break;
    default:
        exit(1);
        break;
    }

    uint64_t fast_access = 0;
    uint64_t slow_access = 0;
    uint64_t cycle = 1;
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    uint64_t unswappedhotpage_count = 0;
    //uint8_t active_hotqueue = 0;                      // 0 or 1
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
    {
        uint64_t current_page_number = _benchmark->memorytrace[i].page_number;
        uint64_t set_number = current_page_number % _memorystructure->set_size;
        if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
        {
            fast_access++;
            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                unswappedhotpage_count++;
            }
        }
        else
        {
            slow_access++;

            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                swappedhotpage_count++;
            }
        }

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit == slow)
            {
                uint64_t temp;
                uint8_t swap_tag = 0;

                switch (set_associative)
                {
                case Direct_Mapped:
                    temp = ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number;
                    if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                    {
                        swap_tag = 1; // start swapping
                        ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number = current_page_number;
                    }
                    break;
                case Two_Way:
                    for (uint8_t j = 0; j < Two_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Four_Way:
                    for (uint8_t j = 0; j < Four_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Eight_Way:
                    for (uint8_t j = 0; j < Eight_Way; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            break;
                        }
                    }
                    break;
                case Fully_Associative:
                    for (uint64_t j = 0; j < _memorystructure->set_size; j++) // find if there has a cold page
                    {
                        temp = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                        if (_memorystructure->pagemetadata[temp].hot_bit == cold)
                        {
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                            break;
                        }
                    }
                    break;
                default:
                    exit(1);
                    break;
                }

                if (swap_tag == 1) // continuing swapping
                {
                    _memorystructure->pagemetadata[temp].fast_bit = slow;
                    _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
                    swap_count++;
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    if (_memorystructure->pagemetadata[j].hot_bit == hot)
                    {
                        _memorystructure->pagemetadata[j].hot_bit = cold; // mark new cold page

                        switch (set_associative)
                        {
                        case Direct_Mapped:
                            break;
                        case Two_Way:
                            break;
                        case Four_Way:
                            break;
                        case Eight_Way:
                            break;
                        case Fully_Associative:
                            break;
                        default:
                            exit(1);
                            break;
                        }
                    }
                }
            }
        }
        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / (unswappedhotpage_count + swappedhotpage_count);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", set_associative, fast_access, slow_access, hit_rate, swap_count, _benchmark->conflictmiss_rate);
}

// Only LRU replacement policy
void hybrid_simulation3(BenchmarkType *_benchmark, MemoryStructureType *_memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    // fill page numbers into the fast memory
    switch (set_associative)
    {
    case Direct_Mapped:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number = i;
            _memorystructure->pagemetadata[i].fast_bit = fast;
        }
        break;
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

    uint64_t fast_access = 0;
    uint64_t slow_access = 0;
    uint64_t cycle = 1;
    uint64_t swap_count = 0;
    uint64_t swappedhotpage_count = 0;
    uint64_t unswappedhotpage_count = 0;
    //uint8_t active_hotqueue = 0;                      // 0 or 1
    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
    {
        uint64_t current_page_number = _benchmark->memorytrace[i].page_number;
        uint64_t set_number = current_page_number % _memorystructure->set_size;
        if (_memorystructure->pagemetadata[current_page_number].fast_bit == fast)
        {
            fast_access++;
            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                unswappedhotpage_count++;
            }

            // Update LRU states
            uint64_t current_page_track = 0;
            uint64_t current_page_way = 0;
            uint8_t update_flag = 0;
            switch (set_associative)
            {
            case Direct_Mapped:
                break;
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
                    printf("Error: swap1 error");
                    exit(1);
                }
                break;
            case Four_Way:
                for (uint8_t j = 0; j < Four_Way; j++)
                {
                    if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == current_page_number)
                    {
                        current_page_track = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j];
                        if (current_page_track != position_one)
                        {
                            //((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                            current_page_way = j;
                            update_flag = 1;
                            break;
                        }
                    }
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {
                    for (uint8_t j = 0; j < Four_Way; j++)
                    {
                        // if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] == current_page_track - 1)
                        // {
                        //     ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] = current_page_track;
                        //     break;
                        // }
                        if ((((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] < current_page_track) && (j != current_page_way))
                        {
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j]++;
                        }
                    }
                }
                break;
            case Eight_Way:
                for (uint8_t j = 0; j < Eight_Way; j++)
                {
                    if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] == current_page_number)
                    {
                        current_page_track = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j];
                        if (current_page_track != position_one)
                        {
                            //((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                            current_page_way = j;
                            update_flag = 1;
                            break;
                        }
                    }
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {
                    for (uint8_t j = 0; j < Eight_Way; j++)
                    {
                        // if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] == current_page_track - 1)
                        // {
                        //     ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] = current_page_track;
                        //     break;
                        // }
                        if ((((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] < current_page_track) && (j != current_page_way))
                        {
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j]++;
                        }
                    }
                }
                break;
            case Fully_Associative:
                for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                {
                    if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN == current_page_number)
                    {
                        current_page_track = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track;
                        if (current_page_track != position_one)
                        {
                            //((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track -= 1;
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track = position_one;
                            current_page_way = j;
                            update_flag = 1;
                            break;
                        }
                    }
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {
                    for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                    {
                        // if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track == current_page_track - 1)
                        // {
                        //     ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track = current_page_track;
                        //     break;
                        // }
                        if ((((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track < current_page_track) && (j != current_page_way))
                        {
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track++;
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

            if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot)
            {
                swappedhotpage_count++;
            }
        }

        if (_memorystructure->pagemetadata[current_page_number].counter < UINT8_MAX)
            _memorystructure->pagemetadata[current_page_number].counter++; // increment its counter

        if (_memorystructure->pagemetadata[current_page_number].counter >= threshold)
            _memorystructure->pagemetadata[current_page_number].hot_bit = hot; // mark hot page

        if (_memorystructure->pagemetadata[current_page_number].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->pagemetadata[current_page_number].fast_bit == slow)
            {
                uint64_t temp;
                uint8_t swap_tag = 0;
                uint64_t least_page_number = 0;
                switch (set_associative)
                {
                case Direct_Mapped:
                    temp = ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number;
                    swap_tag = 1; // start swapping
                    ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[set_number].page_number = current_page_number;

                    break;
                case Two_Way:
                    if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit == position_one)
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1];
                        swap_tag = 1; // start swapping
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[1] = current_page_number;
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_two;
                    }
                    else if (((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit == position_two)
                    {
                        temp = ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0];
                        swap_tag = 1; // start swapping
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].page_number[0] = current_page_number;
                        ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[set_number].old_bit = position_one;
                    }
                    break;
                case Four_Way:
                    for (uint8_t j = 0; j < Four_Way; j++)
                    {
                        if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] == position_four)
                        {
                            temp = ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            //((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;

                            for (uint8_t k = 0; k < Four_Way; k++)
                            {
                                // if (((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[k] == position_four - 1)
                                // {
                                //     ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[k] = position_four;
                                //     break;
                                // }
                                if (k != j)
                                {
                                    ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[k]++;
                                }
                            }
                            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                            break;
                        }
                    }
                    break;
                case Eight_Way:
                    for (uint8_t j = 0; j < Eight_Way; j++)
                    {
                        if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] == position_eight)
                        {
                            temp = ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j];
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].page_number[j] = current_page_number;
                            //((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] -= 1;

                            for (uint8_t k = 0; k < Eight_Way; k++)
                            {
                                // if (((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[k] == position_eight - 1)
                                // {
                                //     ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[k] = position_eight;
                                //     break;
                                // }
                                if (k != j)
                                {
                                    ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[k]++;
                                }
                            }
                            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[set_number].track[j] = position_one;
                            break;
                        }
                    }
                    break;
                case Fully_Associative:
                    uint64_t max_position = _memorystructure->set_size - 1;
                    for (uint64_t j = 0; j < _memorystructure->set_size; j++)
                    {
                        if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track == max_position)
                        {
                            temp = ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN;
                            swap_tag = 1; // start swapping
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].PPN = current_page_number;
                            //((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track -= 1;

                            for (uint64_t k = 0; k < _memorystructure->set_size; k++)
                            {
                                // if (((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].track == max_position - 1)
                                // {
                                //     ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].track = max_position;
                                //     break;
                                // }
                                if (k != j)
                                {
                                    ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[k].track++;
                                }
                            }
                            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[j].track = position_one;
                            break;
                        }
                    }
                    break;
                default:
                    exit(1);
                    break;
                }

                if (swap_tag == 1) // continuing swapping
                {
                    _memorystructure->pagemetadata[temp].fast_bit = slow;
                    _memorystructure->pagemetadata[current_page_number].fast_bit = fast;
                    swap_count++;
                }
            }
        }

        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t j = 0; j < _benchmark->totoalpagelength; j++)
            {
                _memorystructure->pagemetadata[j].counter /= 2;
                if (_memorystructure->pagemetadata[j].counter < threshold)
                {
                    if (_memorystructure->pagemetadata[j].hot_bit == hot)
                    {
                        _memorystructure->pagemetadata[j].hot_bit = cold; // mark new cold page

                        switch (set_associative)
                        {
                        case Direct_Mapped:
                            break;
                        case Two_Way:
                            break;
                        case Four_Way:
                            break;
                        case Eight_Way:
                            break;
                        case Fully_Associative:
                            break;
                        default:
                            exit(1);
                            break;
                        }
                    }
                }
            }
        }
        cycle++;
    }

    float hit_rate = (float)fast_access / (fast_access + slow_access);
    _benchmark->hit_rate = hit_rate;
    _benchmark->conflictmiss_rate = (float)swappedhotpage_count / (unswappedhotpage_count + swappedhotpage_count);
    printf("(Hybrid_simulation.c) set_associative %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count %lld, conflictmiss_rate %f\n", set_associative, fast_access, slow_access, hit_rate, swap_count, _benchmark->conflictmiss_rate);
}