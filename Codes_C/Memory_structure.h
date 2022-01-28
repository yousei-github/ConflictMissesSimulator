#ifndef __MEMORY_STRUCTURE_H
#define __MEMORY_STRUCTURE_H
#include <stdint.h>
#include "DataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        position_one = 0,
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

    typedef struct
    {
        uint8_t counter;
        HotnessType hot_bit;  // whether this page is hot
        LatencyType fast_bit; // inside (1) or outside (0) the fast memory
    } PageMetadataType;

    typedef struct
    {
        uint64_t page_number;
        // no need for LRU replacement policy
    } FastMemoryStructureTypeOne; // Direct_Mapped

    typedef struct
    {
        uint64_t page_number[Two_Way];
        TrackType old_bit;        // LRU replacement policy for Two_Way
    } FastMemoryStructureTypeTwo; // Two_Way

    typedef struct
    {
        uint64_t page_number[Four_Way];
        TrackType track[Four_Way];  // LRU replacement policy for Four_Way
    } FastMemoryStructureTypeThree; // Four_Way

    typedef struct
    {
        uint64_t page_number[Eight_Way];
        TrackType track[Eight_Way]; // LRU replacement policy for Eight_Way
    } FastMemoryStructureTypeFour;  // Eight_Way

    typedef struct
    {
        // no need for virtual page number
        uint64_t PPN;   // physical page number
        uint64_t track; // LRU replacement policy for Fully_Associative
    } PageTableType;

    typedef struct
    {
        PageTableType *page_table;
    } FastMemoryStructureTypeFive; // Fully_Associative

    typedef struct
    {
        TwoLevelRatioType ratio;
        uint8_t threshold; // hotness threshold (0-255)
        uint64_t set_size;
        SetAssociativeType set_associative;
        PageMetadataType *pagemetadata; // store all used pages
        void *fastmemorystructure;      // FastMemoryStructureTypeOne, FastMemoryStructureTypeTwo, FastMemoryStructureTypeThree, FastMemoryStructureTypeFour, FastMemoryStructureTypeFive
    } MemoryStructureType;

    MemoryStructureType *memory_structure_initialization(BenchmarkType *_benchmark, TwoLevelRatioType _ratio, SetAssociativeType _associative, uint8_t _threshold);
    void memory_structure_deallocation(MemoryStructureType *_memorystructure);

#ifdef __cplusplus
}
#endif

#endif
