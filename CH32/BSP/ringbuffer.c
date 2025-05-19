 #include "ringbuffer.h"


void ringbuffer_init(ringbuffer_t *rb,uint32_t buffer_size)
{

    rb->r = 0;
    rb->w = 0;
    rb->buffer_size=buffer_size;
    rb->buffer=(uint8_t*)malloc(sizeof(uint8_t)*buffer_size);
    memset(rb->buffer, 0, sizeof(uint8_t) * rb->buffer_size);
    rb->itemCount = 0;
}


uint8_t ringbuffer_is_full(ringbuffer_t *rb)
{

    return (rb->itemCount == rb->buffer_size);
}


uint8_t ringbuffer_is_empty(ringbuffer_t *rb)
{

    return (rb->itemCount == 0);
}


int8_t ringbuffer_write(ringbuffer_t *rb, uint8_t *data, uint32_t num)
{

    if(ringbuffer_is_full(rb))
        return -1;
        

    while(num--&&rb->itemCount<rb->buffer_size)
    {
        rb->buffer[rb->w] = *data++;
        rb->w = (rb->w + 1) % rb->buffer_size;
        rb->itemCount++;
    }
    
    return 0;
}


int8_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, uint32_t num)
{

    uint8_t temp=0;
    if(ringbuffer_is_empty(rb))
        return -1;
    

    while(num--&&rb->itemCount!=0)
    {
        *data++ = rb->buffer[rb->r];
        rb->r = (rb->r + 1) % rb->buffer_size;
        rb->itemCount--;
        temp++;
    }
    return temp;
}
