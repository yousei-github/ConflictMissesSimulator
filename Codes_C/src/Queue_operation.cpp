#include "Queue_operation.h"
#include <queue>

using namespace std;

extern "C"
{
    void coldqueue_initialization(uint8_t number);
    uint64_t coldqueue_size(uint8_t number);
    uint8_t coldqueue_empty(uint8_t number);
    void queue_push(uint8_t number, uint64_t element);
    uint64_t queue_front(uint8_t number);
    void hotqueue_initialization(uint8_t number);
    uint64_t hotqueue_size(uint8_t number);
    uint8_t hotqueue_empty(uint8_t number);
    void hotqueue_enqueue(uint8_t number, uint64_t element);
    uint64_t hotqueue_dequeue(uint8_t number);
}

queue<uint64_t> ColdQueue[QUEUE_NUMBER];
queue<uint64_t> HotQueue[QUEUE_NUMBER];

void coldqueue_initialization(uint8_t number)
{
    while (ColdQueue[number].empty() != true)
    {
        ColdQueue[number].pop();
    }
}
void hotqueue_initialization(uint8_t number)
{
    while (HotQueue[number].empty() != true)
    {
        HotQueue[number].pop();
    }
}

uint64_t coldqueue_size(uint8_t number)
{
    return ColdQueue[number].size();
}
uint64_t hotqueue_size(uint8_t number)
{
    return HotQueue[number].size();
}

uint8_t coldqueue_empty(uint8_t number)
{
    return ColdQueue[number].empty();
}
uint8_t hotqueue_empty(uint8_t number)
{
    return HotQueue[number].empty();
}

void coldqueue_enqueue(uint8_t number, uint64_t element)
{
    ColdQueue[number].push(element);
}
void hotqueue_enqueue(uint8_t number, uint64_t element)
{
    HotQueue[number].push(element);
}

uint64_t coldqueue_dequeue(uint8_t number)
{
    uint64_t temp = ColdQueue[number].front();
    ColdQueue[number].pop();
    return temp;
}
uint64_t hotqueue_dequeue(uint8_t number)
{
    uint64_t temp = HotQueue[number].front();
    HotQueue[number].pop();
    return temp;
}
