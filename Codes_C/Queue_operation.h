#ifndef __QUEUE_OPERATION_H
#define __QUEUE_OPERATION_H
#include <stdint.h>

#define QUEUE_NUMBER (8)

#ifdef __cplusplus
extern "C"
{
#endif

void queue_initialization(uint8_t number);
uint64_t queue_size(uint8_t number);
uint8_t queue_empty(uint8_t number);
void queue_enqueue(uint8_t number, uint64_t element);
uint64_t queue_dequeue(uint8_t number);


#ifdef __cplusplus
}
#endif

#endif
