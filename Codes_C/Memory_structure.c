#include "Memory_structure.h"
#include <stdlib.h>

static void fast_memory_structure_initialization(MemoryStructureType *_memorystructure);

MemoryStructureType *memory_structure_initialization(BenchmarkType *_benchmark, TwoLevelRatioType _ratio, SetAssociativeType _associative, uint8_t _threshold)
{
    MemoryStructureType *memorystructure = (MemoryStructureType *)malloc(sizeof(MemoryStructureType));
    if (memorystructure == NULL)
    {
        printf("(Memory_structure.c) Error: Memory allocation failed.\n");
        exit(1);
    }

    memorystructure->ratio = _ratio;
    memorystructure->threshold = _threshold;

    if (_associative != Fully_Associative)
    {
        memorystructure->set_size = _benchmark->length2 / memorystructure->ratio / _associative;
    }
    else
        memorystructure->set_size = _benchmark->length2 / memorystructure->ratio;

    memorystructure->set_associative = _associative;
    memorystructure->pagemetadata = (PageMetadataType *)malloc(sizeof(PageMetadataType) * _benchmark->length2);
    if (memorystructure->pagemetadata == NULL)
    {
        printf("(Memory_structure.c) Error: Memory allocation failed.\n");
        exit(1);
    }

    // clear value
    for (uint64_t i = 0; i < _benchmark->length2; i++)
    {
        memorystructure->pagemetadata->counter = 0;
        memorystructure->pagemetadata->hot_bit = cold;
        memorystructure->pagemetadata->fast_bit = slow;
    }

    switch (_associative)
    {
    case Direct_Mapped:
        memorystructure->fastmemorystructure = malloc(sizeof(FastMemoryStructureTypeOne) * memorystructure->set_size);
        break;
    case Two_Way:
        memorystructure->fastmemorystructure = malloc(sizeof(FastMemoryStructureTypeTwo) * memorystructure->set_size);
        break;
    case Four_Way:
        memorystructure->fastmemorystructure = malloc(sizeof(FastMemoryStructureTypeThree) * memorystructure->set_size);
        break;
    case Eight_Way:
        memorystructure->fastmemorystructure = malloc(sizeof(FastMemoryStructureTypeFour) * memorystructure->set_size);
        break;
    case Fully_Associative:
        memorystructure->fastmemorystructure = malloc(sizeof(FastMemoryStructureTypeFive));
        break;
    default:
        exit(1); // error if comes here
        break;
    }
    if (memorystructure->fastmemorystructure == NULL)
    {
        printf("(Memory_structure.c) Error: Memory allocation failed.\n");
        exit(1);
    }
    if (_associative == Fully_Associative)
    {
        ((FastMemoryStructureTypeFive *)memorystructure->fastmemorystructure)->page_table = malloc(sizeof(PageTableType) * memorystructure->set_size);
        if (((FastMemoryStructureTypeFive *)memorystructure->fastmemorystructure)->page_table == NULL)
        {
            printf("(Memory_structure.c) Error: Memory allocation failed.\n");
            exit(1);
        }
    }

    fast_memory_structure_initialization(memorystructure);

    return memorystructure;
}

void fast_memory_structure_initialization(MemoryStructureType *_memorystructure)
{
    switch (_memorystructure->set_associative)
    {
    case Direct_Mapped:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeOne *)_memorystructure->fastmemorystructure)[i].page_number = 0;
        }

        break;
    case Two_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number1 = 0;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].page_number2 = 0;
            ((FastMemoryStructureTypeTwo *)_memorystructure->fastmemorystructure)[i].old_bit = 0;
        }
        break;
    case Four_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number1 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number2 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number3 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].page_number4 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].track1 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].track2 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].track3 = 0;
            ((FastMemoryStructureTypeThree *)_memorystructure->fastmemorystructure)[i].track4 = 0;
        }
        break;
    case Eight_Way:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number1 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number2 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number3 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number4 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number5 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number6 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number7 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].page_number8 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track1 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track2 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track3 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track4 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track5 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track6 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track7 = 0;
            ((FastMemoryStructureTypeFour *)_memorystructure->fastmemorystructure)[i].track8 = 0;
        }
        break;
    case Fully_Associative:
        for (uint64_t i = 0; i < _memorystructure->set_size; i++)
        {
            ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table[i].PPN = 0;
        }
        ((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->cold_pagenumber = 0;
        break;
    default:
        exit(1); // error if comes here
        break;
    }
}

// remember to free the allocated memory
void memory_structure_deallocation(MemoryStructureType *_memorystructure)
{
    switch (_memorystructure->set_associative)
    {
    case Direct_Mapped:
        free(_memorystructure->fastmemorystructure);
        break;
    case Two_Way:
        free(_memorystructure->fastmemorystructure);
        break;
    case Four_Way:
        free(_memorystructure->fastmemorystructure);
        break;
    case Eight_Way:
        free(_memorystructure->fastmemorystructure);
        break;
    case Fully_Associative:
        free(((FastMemoryStructureTypeFive *)_memorystructure->fastmemorystructure)->page_table);
        free(_memorystructure->fastmemorystructure);
        break;
    default:
        exit(1); // error if comes here
        break;
    }

    free(_memorystructure->pagemetadata);
    free(_memorystructure);
}