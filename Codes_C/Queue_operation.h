#ifndef __QUEUE_OPERATION_H
#define __QUEUE_OPERATION_H
#include <stdint.h>

#define QUEUE_NUMBER (8)

#ifdef __cplusplus
extern "C"
{
#endif

void coldqueue_initialization(uint8_t number);
uint64_t coldqueue_size(uint8_t number);
uint8_t coldqueue_empty(uint8_t number);
void coldqueue_enqueue(uint8_t number, uint64_t element);
uint64_t coldqueue_dequeue(uint8_t number);

void hotqueue_initialization(uint8_t number);
uint64_t hotqueue_size(uint8_t number);
uint8_t hotqueue_empty(uint8_t number);
void hotqueue_enqueue(uint8_t number, uint64_t element);
uint64_t hotqueue_dequeue(uint8_t number);

#ifdef __cplusplus
}
#endif

#endif
