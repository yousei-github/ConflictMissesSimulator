#include "Queue_operation.h"
#include <queue>

using namespace std;

extern "C"
{
    void queue_initialization(uint8_t number);
    uint64_t queue_size(uint8_t number);
    uint8_t queue_empty(uint8_t number);
    void queue_push(uint8_t number, uint64_t element);
    uint64_t queue_front(uint8_t number);
}

queue<uint64_t> ColdQueue[QUEUE_NUMBER];

void queue_initialization(uint8_t number)
{
    while (ColdQueue[number].empty() != true)
    {
        ColdQueue[number].pop();
    }
}

uint64_t queue_size(uint8_t number)
{
    return ColdQueue[number].size();
}

uint8_t queue_empty(uint8_t number)
{
    return ColdQueue[number].empty();
}

void queue_enqueue(uint8_t number, uint64_t element)
{
    ColdQueue[number].push(element);
}

uint64_t queue_dequeue(uint8_t number)
{   
    uint64_t temp = ColdQueue[number].front();
    ColdQueue[number].pop();
    return temp;
}