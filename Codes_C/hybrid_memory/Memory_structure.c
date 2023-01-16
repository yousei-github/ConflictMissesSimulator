#include "Memory_structure.h"
#include "Configuration.h"

static void set_structure_initialization(MemoryStructureType* _memorystructure);

/**
 * @param: fastmemory_capacity (byte unit)
 *
 *
 */
MemoryStructureType* memory_structure_initialization(BenchmarkType* _benchmark, uint64_t fastmemory_capacity, SetAssociativeType _associative, uint8_t _threshold, MigrationGranularityType migration_granularity)
{
    MemoryStructureType* memorystructure = (MemoryStructureType*)malloc(sizeof(MemoryStructureType));
    if (memorystructure == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    memorystructure->fastmemory_capacity = fastmemory_capacity;
    memorystructure->threshold = _threshold; // load hotness threshold
    memorystructure->migration_granularity = migration_granularity;
    memorystructure->set_associative = _associative; // load set associative

    // calculate the total number of sets in fast memory
    if (memorystructure->set_associative != Fully_Associative)
    {
        memorystructure->total_set_number = memorystructure->fastmemory_capacity / memorystructure->migration_granularity / memorystructure->set_associative;
    }
    else
        memorystructure->total_set_number = memorystructure->fastmemory_capacity / memorystructure->migration_granularity;

    // load the data structure to manage the data in fast memory
    switch (memorystructure->set_associative)
    {
    case Direct_Mapped:
        memorystructure->set_structure = malloc(sizeof(DirectMappedSetType) * memorystructure->total_set_number);
        break;
    case Two_Way:
        memorystructure->set_structure = malloc(sizeof(TwoWaySetType) * memorystructure->total_set_number);
        break;
    case Four_Way:
        memorystructure->set_structure = malloc(sizeof(FourWaySetType) * memorystructure->total_set_number);
        break;
    case Eight_Way:
        memorystructure->set_structure = malloc(sizeof(EightWaySetType) * memorystructure->total_set_number);
        break;
    case Fully_Associative:
        memorystructure->set_structure = malloc(sizeof(FullyAssociativeSetType));
        break;
    default:
        exit(1); // error if comes here
        break;
    }
    if (memorystructure->set_structure == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }
    if (memorystructure->set_associative == Fully_Associative) // special data structure for fully associative
    {
        ((FullyAssociativeSetType*)memorystructure->set_structure)->segment_table = (SegmentTableType*)malloc(sizeof(SegmentTableType) * memorystructure->total_set_number);
        if (((FullyAssociativeSetType*)memorystructure->set_structure)->segment_table == NULL)
        {
            printf("(%s) Error: Memory allocation failed.\n", __func__);
            exit(1);
        }
    }

    // initialize the set structure
    set_structure_initialization(memorystructure);

    memorystructure->total_segment_number = _benchmark->total_page_number * ((float)PAGE_SIZE / memorystructure->migration_granularity);

    memorystructure->segment_metadata = (SegmentMetadataType*)malloc(sizeof(SegmentMetadataType) * memorystructure->total_segment_number); // load all segments' metadata
    if (memorystructure->segment_metadata == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    // initialize value for each segment's metadata
    for (uint64_t i = 0; i < memorystructure->total_segment_number; i++)
    {
        memorystructure->segment_metadata[i].segment_address = 0;
        memorystructure->segment_metadata[i].counter = 0;
        memorystructure->segment_metadata[i].hot_bit = cold;
        memorystructure->segment_metadata[i].fast_bit = slow;
    }
    memorystructure->segment_metadata_length = 0;

    memorystructure->total_cache_line_number = _benchmark->total_page_number * CACHE_LINE_NUMBER_IN_PAGE;
    memorystructure->cache_line_metadata = (CacheLineMetadataType*)malloc(sizeof(CacheLineMetadataType) * memorystructure->total_cache_line_number); // load all cache lines' metadata
    if (memorystructure->cache_line_metadata == NULL)
    {
        printf("(%s) Error: Memory allocation failed.\n", __func__);
        exit(1);
    }

    // initialize value for each cache line's metadata
    for (uint64_t i = 0; i < memorystructure->total_cache_line_number; i++)
    {
        memorystructure->cache_line_metadata[i].access_bit = clear;
    }
    memorystructure->cache_line_metadata_length = 0;

    memorystructure->segment_size_at_cache_line = memorystructure->migration_granularity / CACHE_LINE;

    return memorystructure;
}

void set_structure_initialization(MemoryStructureType* _memorystructure)
{
    switch (_memorystructure->set_associative)
    {
    case Direct_Mapped:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            ((DirectMappedSetType*)_memorystructure->set_structure)[set].segment_address = 0;
        }
        break;
    case Two_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            ((TwoWaySetType*)_memorystructure->set_structure)[set].segment_address[0] = 0;
            ((TwoWaySetType*)_memorystructure->set_structure)[set].segment_address[1] = 0;
            ((TwoWaySetType*)_memorystructure->set_structure)[set].track[0] = position_one;
            ((TwoWaySetType*)_memorystructure->set_structure)[set].track[1] = position_one;
        }
        break;
    case Four_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            for (uint8_t way = 0; way < Four_Way; way++)
            {
                ((FourWaySetType*)_memorystructure->set_structure)[set].segment_address[way] = 0;
                ((FourWaySetType*)_memorystructure->set_structure)[set].track[way] = position_one;
            }
        }
        break;
    case Eight_Way:
        for (uint64_t set = 0; set < _memorystructure->total_set_number; set++)
        {
            for (uint8_t way = 0; way < Eight_Way; way++)
            {
                ((EightWaySetType*)_memorystructure->set_structure)[set].segment_address[way] = 0;
                ((EightWaySetType*)_memorystructure->set_structure)[set].track[way] = position_one;
            }
        }
        break;
    case Fully_Associative:
        for (uint64_t way = 0; way < _memorystructure->total_set_number; way++)
        {
            ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].segment_address = 0;
            ((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table[way].track = position_one; // here is different from other set-associative structure
        }
        break;
    default:
        exit(1); // error if comes here
        break;
    }
}

// remember to free the allocated memory
void memory_structure_deallocation(MemoryStructureType* _memorystructure)
{
    switch (_memorystructure->set_associative)
    {
    case Direct_Mapped:
        free(_memorystructure->set_structure);
        break;
    case Two_Way:
        free(_memorystructure->set_structure);
        break;
    case Four_Way:
        free(_memorystructure->set_structure);
        break;
    case Eight_Way:
        free(_memorystructure->set_structure);
        break;
    case Fully_Associative:
        free(((FullyAssociativeSetType*)_memorystructure->set_structure)->segment_table);
        free(_memorystructure->set_structure);
        break;
    default:
        exit(1); // error if comes here
        break;
    }

    free(_memorystructure->segment_metadata);
    free(_memorystructure->cache_line_metadata);
    free(_memorystructure);
}