#ifndef __MEMORY_STRUCTURE_H
#define __MEMORY_STRUCTURE_H
#include <stdint.h>
#include "DataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_SETASSOCIATIVE_TYPE (5)
#define MAX_MIGRATIONGRANULARITY_TYPE (7)

    typedef enum
    {
        position_one = 0, // newest one
        position_two = 1, // two way uses above
        position_three = 2,
        position_four = 3, // four way uses above
        position_five = 4,
        position_six = 5,
        position_seven = 6,
        position_eight = 7, // eight way uses above
    } TrackType;            // the higher position is less recently used (the highest position is least recently used)

    typedef enum
    {
        Ratio_1_to_3 = 4,   // 1:3
        Ratio_1_to_4 = 5,   // 1:4
        Ratio_1_to_8 = 9,   // 1:8
        Ratio_1_to_16 = 17, // 1:16
    } TwoLevelRatioType;

    typedef enum
    {
        Direct_Mapped = 1,
        Two_Way = 2,
        Four_Way = 4,
        Eight_Way = 8,
        Fully_Associative = 0,
    } SetAssociativeType;

    typedef enum
    {
        Byte_64 = 64,
        Byte_128 = 128,
        Byte_256 = 256,
        Byte_512 = 512,
        Byte_1024 = 1024,
        Byte_2048 = 2048,
        Byte_4096 = 4096,
    } MigrationGranularityType;

    typedef struct
    {
        uint64_t segment_address; // used to record the segment address
        uint8_t counter;
        HotnessType hot_bit;  // whether this segment is hot
        LatencyType fast_bit; // inside (1) or outside (0) the fast memory
    } SegmentMetadataType;    // metadata for each segment used

    typedef struct
    {
        AccessType access_bit; // whether this cache line is accessed (in fast memory)
    } CacheLineMetadataType;   // metadata for each cache line used, it is referenced according to the SegmentMetadataType

    typedef struct
    {
        uint64_t segment_address; // segment_address[set_associative]
        // no need for LRU replacement policy
    } DirectMappedSetType; // Direct_Mapped

    typedef struct
    {
        uint64_t segment_address[Two_Way];
        TrackType track[Two_Way]; // LRU replacement policy for Two_Way
    } TwoWaySetType;              // Two_Way

    typedef struct
    {
        uint64_t segment_address[Four_Way];
        TrackType track[Four_Way]; // LRU replacement policy for Four_Way
    } FourWaySetType;              // Four_Way

    typedef struct
    {
        uint64_t segment_address[Eight_Way];
        TrackType track[Eight_Way]; // LRU replacement policy for Eight_Way
    } EightWaySetType;              // Eight_Way

    typedef struct
    {
        uint64_t segment_address; // segment address
        TrackType track;          // LRU replacement policy for Fully_Associative
    } SegmentTableType;

    typedef struct
    {
        SegmentTableType *segment_table;
    } FullyAssociativeSetType; // Fully_Associative

    typedef struct
    {
        TwoLevelRatioType ratio;
        uint64_t fastmemory_capacity; // fast memory capacity at byte unit
        uint8_t threshold;            // hotness threshold (0-255)
        MigrationGranularityType migration_granularity;
        SetAssociativeType set_associative;

        uint64_t total_set_number; // total number of sets in fast memory
        void *set_structure;

        uint64_t total_segment_number;
        uint64_t segment_metadata_length;      // used to update SegmentMetadataType *segment_metadata, it should be less than or equal to total_segment_number after finishing the simulation
        SegmentMetadataType *segment_metadata; // a buffer which stores all used segments' metadata

        uint64_t total_cache_line_number;
        uint64_t cache_line_metadata_length;        // used to update CacheLineMetadataType *cache_line_metadata, it should be less than or equal to total_cache_line_number after finishing the simulation
        CacheLineMetadataType *cache_line_metadata; // a buffer which stores all cache lines' metadata, it is referenced according to the SegmentMetadataType *segment_metadata
    
        uint64_t segment_size_at_cache_line;
    } MemoryStructureType;

    // MemoryStructureType *memory_structure_initialization(BenchmarkType *_benchmark, TwoLevelRatioType _ratio, SetAssociativeType _associative, uint8_t _threshold);
    MemoryStructureType *memory_structure_initialization(BenchmarkType *_benchmark, uint64_t fastmemory_capacity, SetAssociativeType _associative, uint8_t _threshold, MigrationGranularityType migration_granularity);
    void memory_structure_deallocation(MemoryStructureType *_memorystructure);

#ifdef __cplusplus
}
#endif

#endif
