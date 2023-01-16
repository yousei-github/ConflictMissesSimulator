#include "Hybrid_simulation.h"
#include "Configuration.h"

#define INTERVAL_FOR_DECREMENT (1000) // unit: cycle
#define INTERVAL_FOR_PRINT (10000)    // unit: cycle

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

static uint8_t check_segment_address_in_segment_metadata(MemoryStructureType* _memorystructure, uint64_t current_segment_address, uint64_t* position_in_segment_metadata);
static uint64_t mark_slow_in_segment_metadata(MemoryStructureType* _memorystructure, uint64_t segment_address);

// Only LRU replacement policy
void hybrid_simulation_usingLRU(BenchmarkType* _benchmark, MemoryStructureType* _memorystructure)
{
    uint8_t threshold = _memorystructure->threshold;
    SetAssociativeType set_associative = _memorystructure->set_associative;

    // fill segment base address into the fast memory
    switch (set_associative)
    {
    case Direct_Mapped:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            ((DirectMappedSetType*)_memorystructure->set_structure)[set].segment_address = set;
        }
        break;
    case Two_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            for (uint8_t way = 0; way < Two_Way; way++) // go through the ways
            {
                ((TwoWaySetType*)_memorystructure->set_structure)[set].segment_address[way] = set + _memorystructure->total_set_number * way;
                ((TwoWaySetType*)_memorystructure->set_structure)[set].track[way] = position_one + way;
            }
        }
        break;
    case Four_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            for (uint8_t way = 0; way < Four_Way; way++) // go through the ways
            {
                ((FourWaySetType*)_memorystructure->set_structure)[set].segment_address[way] = set + _memorystructure->total_set_number * way;
                ((FourWaySetType*)_memorystructure->set_structure)[set].track[way] = position_one + way;
            }
        }
        break;
    case Eight_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            for (uint8_t way = 0; way < Eight_Way; way++) // go through the ways
            {
                ((EightWaySetType*)_memorystructure->set_structure)[set].segment_address[way] = set + _memorystructure->total_set_number * way;
                ((EightWaySetType*)_memorystructure->set_structure)[set].track[way] = position_one + way;
            }
        }
        break;
    case Fully_Associative:
        for (uint64_t way = 0; way < _memorystructure->total_set_number; way++)
        {
            ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].segment_address = way;
            ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].track = position_one + way;
        }
        break;
    default:
        exit(1);
        break;
    }

    uint64_t fast_access = 0; // count the number of access in fast memory
    uint64_t slow_access = 0; // count the number of access in slow memory
    uint64_t cycle = 0;
    uint64_t swap_count = 0;                         // count the swapping time
    uint64_t access_swapped_hot_segment_count = 0;   // count the number of hot segments accessed in slow memory
    uint64_t access_unswapped_hot_segment_count = 0; // count the number of hot segments accessed in fast memory
    uint64_t unuse_migrated_cache_line_NM_number = 0;

#if LIMIT_TRACE_READ_NUMBER == ENABLE
    if (_benchmark->tracelength > MAX_TRACE_READ_NUMBER)
    {
        _benchmark->tracelength = MAX_TRACE_READ_NUMBER;
    }
#endif

    for (uint64_t i = 0; i < _benchmark->tracelength; i++) // go through all memory requests
    {
        uint64_t current_segment_address = 0;
        uint64_t current_cache_line_address = 0;

        // calculate current segment address and current cache line address
        switch (_benchmark->trace_type)
        {
        case cache_line:
        {
            current_segment_address = _benchmark->memorytrace[i].address / (_memorystructure->migration_granularity / CACHE_LINE);
            current_cache_line_address = _benchmark->memorytrace[i].address;
        }
        break;
        case page:
        {
            current_segment_address = _benchmark->memorytrace[i].address;
            current_cache_line_address = 0;
        }
        break;
        default:
            exit(1);
            break;
        }

        // calculate the set number
        uint64_t set_number = current_segment_address % _memorystructure->total_set_number;
        uint64_t position_in_segment_metadata = 0;
        uint64_t position_in_cache_line_metadata = 0;

        // check whether this segment address is new segment address which is not in _memorystructure->segment_metadata
        uint8_t new_flag = 1;
        new_flag = check_segment_address_in_segment_metadata(_memorystructure, current_segment_address, &position_in_segment_metadata);

        // get a new segment address which is not in _memorystructure->segment_metadata
        if (new_flag)
        {
            position_in_segment_metadata = _memorystructure->segment_metadata_length;
            _memorystructure->segment_metadata[position_in_segment_metadata].segment_address = current_segment_address;
            _memorystructure->segment_metadata_length++;
            _memorystructure->cache_line_metadata_length += _memorystructure->segment_size_at_cache_line;
        }

        // check whether this segment address is already in fast memory but its fast bit is not set
        uint8_t in_fast_flag = 0;
        uint64_t current_way = 0;
        switch (set_associative)
        {
        case Direct_Mapped:
            if (((DirectMappedSetType*)_memorystructure->set_structure)[set_number].segment_address == current_segment_address)
            {
                in_fast_flag = 1;
            }
            break;
        case Two_Way:
            for (uint8_t way = 0; way < Two_Way; way++) // go through the ways
            {
                if (((TwoWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way] == current_segment_address)
                {
                    in_fast_flag = 1;
                    current_way = way;
                    break;
                }
            }
            break;
        case Four_Way:
            for (uint8_t way = 0; way < Four_Way; way++) // go through the ways
            {
                if (((FourWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way] == current_segment_address)
                {
                    in_fast_flag = 1;
                    current_way = way;
                    break;
                }
            }
            break;
        case Eight_Way:
            for (uint8_t way = 0; way < Eight_Way; way++) // go through the ways
            {
                if (((EightWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way] == current_segment_address)
                {
                    in_fast_flag = 1;
                    current_way = way;
                    break;
                }
            }
            break;
        case Fully_Associative:
            uint8_t find_flag = 0;

            int64_t way;
#if USE_OPENMP == ENABLE
#pragma omp parallel
            {
#pragma omp for
#endif
                for (way = 0; way < _memorystructure->total_set_number; way++)
                {
                    if (((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].segment_address == current_segment_address)
                    {
                        find_flag = 1;
                        in_fast_flag = 1;
                        current_way = way;
                        break;
                    }
#if USE_OPENMP == ENABLE
                    if (find_flag == 1) // used for other threads that if find_flag is already set
                    {
                        break;
                    }
#endif
                }
#if USE_OPENMP == ENABLE
            }
#endif
            break;
        default:
            exit(1);
            break;
        }

        // this segment address is already in fast memory
        if (in_fast_flag)
        {
            // but its fast bit is not set
            if (_memorystructure->segment_metadata[position_in_segment_metadata].fast_bit == slow)
            {
                _memorystructure->segment_metadata[position_in_segment_metadata].fast_bit = fast;
            }
        }

        // calculate position_in_cache_line_metadata
        position_in_cache_line_metadata = position_in_segment_metadata * _memorystructure->segment_size_at_cache_line + current_cache_line_address % _memorystructure->segment_size_at_cache_line;

        if (_memorystructure->segment_metadata[position_in_segment_metadata].fast_bit == fast)
        {
            fast_access++;

            if (_memorystructure->segment_metadata[position_in_segment_metadata].hot_bit == hot)
            {
                access_unswapped_hot_segment_count++;
            }

            // mark the cache line which is accessed in fast memory
            if (_memorystructure->cache_line_metadata[position_in_cache_line_metadata].access_bit == clear)
            {
                _memorystructure->cache_line_metadata[position_in_cache_line_metadata].access_bit = set;
            }

            // Update LRU states
            uint64_t current_segment_track = 0;
            uint8_t update_flag = 0;
            switch (set_associative)
            {
            case Direct_Mapped:
                break;
            case Two_Way:
                ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[current_way] = position_one;
                if (current_way) // if current_way == 1
                {
                    ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[0] = position_two;
                }
                else // if current_way == 0
                {
                    ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[1] = position_two;
                }
                break;
            case Four_Way:
                current_segment_track = ((FourWaySetType*)_memorystructure->set_structure)[set_number].track[current_way];
                if (current_segment_track != position_one)
                {
                    ((FourWaySetType*)_memorystructure->set_structure)[set_number].track[current_way] = position_one;
                    update_flag = 1;
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {
                    for (uint8_t way = 0; way < Four_Way; way++)
                    {
                        if ((((FourWaySetType*)_memorystructure->set_structure)[set_number].track[way] < current_segment_track) && (way != current_way))
                        {
                            ((FourWaySetType*)_memorystructure->set_structure)[set_number].track[way]++;
                        }
                    }
                }
                break;
            case Eight_Way:
                current_segment_track = ((EightWaySetType*)_memorystructure->set_structure)[set_number].track[current_way];
                if (current_segment_track != position_one)
                {
                    ((EightWaySetType*)_memorystructure->set_structure)[set_number].track[current_way] = position_one;
                    update_flag = 1;
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {
                    for (uint8_t way = 0; way < Eight_Way; way++)
                    {
                        if ((((EightWaySetType*)_memorystructure->set_structure)[set_number].track[way] < current_segment_track) && (way != current_way))
                        {
                            ((EightWaySetType*)_memorystructure->set_structure)[set_number].track[way]++;
                        }
                    }
                }
                break;
            case Fully_Associative:
                current_segment_track = ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[current_way].track;
                if (current_segment_track != position_one)
                {
                    ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[current_way].track = position_one;
                    update_flag = 1;
                }
                if (update_flag == 1) // update the page's LRU states that is same as the current page's
                {

                    int64_t way;
#if USE_OPENMP == ENABLE
#pragma omp parallel
                    {
#pragma omp for
#endif
                        for (way = 0; way < _memorystructure->total_set_number; way++)
                        {
                            if ((((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].track < current_segment_track) && (way != current_way))
                            {
                                ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].track++;
                            }
                        }
#if USE_OPENMP == ENABLE
                    }
#endif
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

            if (_memorystructure->segment_metadata[position_in_segment_metadata].hot_bit == hot)
            {
                access_swapped_hot_segment_count++;
            }
        }

        if (_memorystructure->segment_metadata[position_in_segment_metadata].counter < UINT8_MAX)
            _memorystructure->segment_metadata[position_in_segment_metadata].counter++; // increment its counter

        if (_memorystructure->segment_metadata[position_in_segment_metadata].counter >= threshold)
            _memorystructure->segment_metadata[position_in_segment_metadata].hot_bit = hot; // mark hot page

        if (_memorystructure->segment_metadata[position_in_segment_metadata].hot_bit == hot) // swap if the hot page is in slow memory
        {
            if (_memorystructure->segment_metadata[position_in_segment_metadata].fast_bit == slow)
            {
                uint64_t temp;
                uint8_t swap_tag = 0;
                // Update LRU states
                switch (set_associative)
                {
                case Direct_Mapped:
                    temp = ((DirectMappedSetType*)_memorystructure->set_structure)[set_number].segment_address;
                    swap_tag++; // start swapping
                    ((DirectMappedSetType*)_memorystructure->set_structure)[set_number].segment_address = current_segment_address;
                    break;
                case Two_Way:
                    if (((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[0] == position_one)
                    {
                        temp = ((TwoWaySetType*)_memorystructure->set_structure)[set_number].segment_address[1];
                        swap_tag++; // start swapping
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].segment_address[1] = current_segment_address;
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[0] = position_two;
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[1] = position_one;
                    }
                    else if (((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[1] == position_one)
                    {
                        temp = ((TwoWaySetType*)_memorystructure->set_structure)[set_number].segment_address[0];
                        swap_tag++; // start swapping
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].segment_address[0] = current_segment_address;
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[0] = position_one;
                        ((TwoWaySetType*)_memorystructure->set_structure)[set_number].track[1] = position_two;
                    }
                    break;
                case Four_Way:
                    for (uint8_t way = 0; way < Four_Way; way++)
                    {
                        if (((FourWaySetType*)_memorystructure->set_structure)[set_number].track[way] == position_four)
                        {
                            temp = ((FourWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way];
                            swap_tag++; // start swapping
                            ((FourWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way] = current_segment_address;

                            for (uint8_t k = 0; k < Four_Way; k++)
                            {
                                if (k != way)
                                {
                                    ((FourWaySetType*)_memorystructure->set_structure)[set_number].track[k]++;
                                }
                            }
                            ((FourWaySetType*)_memorystructure->set_structure)[set_number].track[way] = position_one;
                            break;
                        }
                    }
                    break;
                case Eight_Way:
                    for (uint8_t way = 0; way < Eight_Way; way++)
                    {
                        if (((EightWaySetType*)_memorystructure->set_structure)[set_number].track[way] == position_eight)
                        {
                            temp = ((EightWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way];
                            swap_tag++; // start swapping
                            ((EightWaySetType*)_memorystructure->set_structure)[set_number].segment_address[way] = current_segment_address;

                            for (uint8_t k = 0; k < Eight_Way; k++)
                            {
                                if (k != way)
                                {
                                    ((EightWaySetType*)_memorystructure->set_structure)[set_number].track[k]++;
                                }
                            }
                            ((EightWaySetType*)_memorystructure->set_structure)[set_number].track[way] = position_one;
                            break;
                        }
                    }
                    break;
                case Fully_Associative:
                    uint8_t find_flag = 0;
                    uint64_t new_way = 0;
                    uint64_t max_position = _memorystructure->total_set_number - 1;

                    int64_t way;
#if USE_OPENMP == ENABLE
#pragma omp parallel
                    {
#pragma omp for
#endif
                        for (way = 0; way < _memorystructure->total_set_number; way++)
                        {
                            if (((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].track == max_position)
                            {
                                find_flag++;
                                new_way = way; // mark the way to swap
                                temp = ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[new_way].segment_address;
                                swap_tag++; // start swapping
                                ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[new_way].segment_address = current_segment_address;

                                // here we don't update other segments' LRU states (for parallel)

                                ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[new_way].track = position_one;
                                break;
                            }
#if USE_OPENMP == ENABLE
                            if (find_flag) // used for other threads that if find_flag is already set
                            {
                                break;
                            }
#endif
                        }
#if USE_OPENMP == ENABLE
                    }
#endif
                    if (find_flag == 1)
                    {
                        // here we update other segments' LRU states
                        int64_t k;
#if USE_OPENMP == ENABLE
#pragma omp parallel
                        {
#pragma omp for
#endif
                            for (k = 0; k < _memorystructure->total_set_number; k++)
                            {
                                if (k != new_way)
                                {
                                    ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[k].track++;
                                }
                            }
#if USE_OPENMP == ENABLE
                        }
#endif
                    }
                    else
                    {
                        printf("(%s) Error at find_flag != 1, %d.\n", __func__, find_flag);
                    }

                    break;
                default:
                    exit(1);
                    break;
                }

                if (swap_tag == 1) // continuing swapping
                {
                    unuse_migrated_cache_line_NM_number += mark_slow_in_segment_metadata(_memorystructure, temp);
                    _memorystructure->segment_metadata[position_in_segment_metadata].fast_bit = fast;
                    swap_count++;
                }
                else
                {
                    printf("(%s) Error at swap_tag != 1, %d.\n", __func__, swap_tag);
                }
            }
        }

#if DECREASE_COUNTER == ENABLE
        // decrease all pages' counter per INTERVAL_FOR_DECREMENT cycles
        if (cycle % INTERVAL_FOR_DECREMENT == 0)
        {
            for (uint64_t i = 0; i < _memorystructure->segment_metadata_length; i++)
            {
                _memorystructure->segment_metadata[i].counter /= 2;
                if (_memorystructure->segment_metadata[i].counter < threshold)
                {
                    if (_memorystructure->segment_metadata[i].hot_bit == hot)
                    {
                        _memorystructure->segment_metadata[i].hot_bit = cold; // mark new cold page
                    }
                }
            }
        }
#endif

#if OUTPUT_DETAIL == ENABLE
        if (cycle % INTERVAL_FOR_PRINT == 0)
        {
            printf("(%s) cycle: %lld, trace number: %lld, total trace: %lld, unuse_migrated_cache_line_NM_number: %lld.\n", __func__, cycle, i, _benchmark->tracelength, unuse_migrated_cache_line_NM_number);
        }

#endif
        cycle++;
    }

    _benchmark->hit_rate = safe_division_float(fast_access, fast_access + slow_access);
    _benchmark->conflict_miss_rate = safe_division_float(access_swapped_hot_segment_count, access_unswapped_hot_segment_count + access_swapped_hot_segment_count);
    _benchmark->migration_data_NM_to_FM_in_byte = swap_count * _memorystructure->migration_granularity;
    _benchmark->unuse_migrated_cache_line_NM_number = unuse_migrated_cache_line_NM_number;
    printf("(%s) set_associative: %d, migration_granularity: %d, fast_access: %lld, slow_access: %lld, hit_rate: %f, swap_count: %lld, conflict_miss_rate: %f, migration_data_NM_to_FM_in_byte: %lld.\n", __func__,
           set_associative, _memorystructure->migration_granularity, fast_access, slow_access, _benchmark->hit_rate, swap_count, _benchmark->conflict_miss_rate, _benchmark->migration_data_NM_to_FM_in_byte);
    printf("unuse_migrated_cache_line_NM_number: %lld.\n", _benchmark->unuse_migrated_cache_line_NM_number);
}

uint8_t check_segment_address_in_segment_metadata(MemoryStructureType* _memorystructure, uint64_t current_segment_address, uint64_t* position_in_segment_metadata)
{
    uint8_t new_flag = 1;

    int64_t i = 0;
#if USE_OPENMP == ENABLE
#pragma omp parallel
    {
#pragma omp for
#endif
        for (i = 0; i < _memorystructure->segment_metadata_length; i++)
        {
            if (_memorystructure->segment_metadata[i].segment_address == current_segment_address)
            {
                new_flag = 0; // old segment address
                *position_in_segment_metadata = i;
                break;
            }
#if USE_OPENMP == ENABLE
            if (new_flag == 0) // used for other threads that if new_flag is already clear
            {
                break;
            }
#endif
        }
#if USE_OPENMP == ENABLE
    }
#endif

    return new_flag;
}

uint64_t mark_slow_in_segment_metadata(MemoryStructureType* _memorystructure, uint64_t segment_address)
{
    uint8_t slow_flag = 0;
    uint64_t position_in_segment_metadata = 0;
    uint64_t unuse_migrated_cache_line_NM_number = 0; // in case the segment is not used by the application in fast memory

    int64_t i = 0;
#if USE_OPENMP == ENABLE
#pragma omp parallel
    {
#pragma omp for
#endif
        for (i = 0; i < _memorystructure->segment_metadata_length; i++)
        {
            if (_memorystructure->segment_metadata[i].segment_address == segment_address)
            {
                slow_flag = 1;
                if (_memorystructure->segment_metadata[i].fast_bit == slow)
                {
                    printf("(%s) Error in swapping. i = %lld.\n", __func__, i);
                    exit(1);
                }

                _memorystructure->segment_metadata[i].fast_bit = slow;
                position_in_segment_metadata = i;
                unuse_migrated_cache_line_NM_number = _memorystructure->segment_size_at_cache_line;

                break;
            }
#if USE_OPENMP == ENABLE
            if (slow_flag == 1) // used for other threads that if slow_flag is already set
            {
                break;
            }
#endif
        }
#if USE_OPENMP == ENABLE
    }
#endif

    if (slow_flag == 1)
    {
        int64_t position_in_cache_line_metadata;
        int64_t start_position = position_in_segment_metadata * _memorystructure->segment_size_at_cache_line;
#if USE_OPENMP == ENABLE
#pragma omp parallel
        {
#pragma omp for
#endif
            for (position_in_cache_line_metadata = start_position; position_in_cache_line_metadata < start_position + _memorystructure->segment_size_at_cache_line; position_in_cache_line_metadata++)
            {
                // clear the cache line which was accessed in fast memory
                if (_memorystructure->cache_line_metadata[position_in_cache_line_metadata].access_bit == set)
                {
                    _memorystructure->cache_line_metadata[position_in_cache_line_metadata].access_bit = clear;
                    unuse_migrated_cache_line_NM_number--;
                }
            }
#if USE_OPENMP == ENABLE
        }
#endif
    }

    return unuse_migrated_cache_line_NM_number;
}
