#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "bsp.h"


typedef struct {
    uint32_t w;
    uint32_t r;
    uint8_t* buffer;
    uint32_t itemCount;
    uint32_t buffer_size;
}ringbuffer_t;

void ringbuffer_init(ringbuffer_t *rb,uint32_t buffer_size);
uint8_t ringbuffer_is_full(ringbuffer_t *rb);
uint8_t ringbuffer_is_empty(ringbuffer_t *rb);
int8_t ringbuffer_write(ringbuffer_t *rb, uint8_t *data, uint32_t num);
int8_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, uint32_t num);

extern ringbuffer_t usart_rb;

#endif
