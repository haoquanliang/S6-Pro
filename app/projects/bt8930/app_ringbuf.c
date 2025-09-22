#include "app_ringbuf.h"

// 初始化环形缓冲区
void app_ring_buffer_init(ring_buffer *rb) 
{
    rb->head = 0;
    rb->tail = 0;
    rb->size = 0;
}

// 判断队列是否满
bool app_ring_buffer_is_full(ring_buffer *rb, uint16_t data_len) 
{
    // 考虑存储数据长度 (2 bytes) 的额外空间
    return (BUFFER_SIZE - rb->size) < (data_len + 2);
}

// 判断队列是否空
bool app_ring_buffer_is_empty(ring_buffer *rb) 
{
    return rb->size == 0;
}

// 将不定长的数据包添加到队列中
bool app_ring_buffer_enqueue(ring_buffer *rb, uint8_t *data, uint16_t len) 
{
    if (app_ring_buffer_is_full(rb, len)) 
    {
        return false;  // 队列满，无法存储数据
    }

    // 存储数据包的长度 (2 bytes)
    rb->buffer[rb->tail] = (len >> 8) & 0xFF;  // 高字节
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;   // 更新队列尾，防止越界

    rb->buffer[rb->tail] = len & 0xFF;         // 低字节
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;   // 更新队列尾，防止越界

    // 存储数据包内容
    for (uint16_t i = 0; i < len; i++) 
    {
        rb->buffer[rb->tail] = data[i];
        rb->tail = (rb->tail + 1) % BUFFER_SIZE;  // 每次都要防止越界
    }

    rb->size += len + 2;  // 更新队列中数据的总大小
    return true;
}

// 从队列中取出数据包
bool app_ring_buffer_dequeue(ring_buffer *rb, uint8_t *data, uint16_t *len) 
{
    if (app_ring_buffer_is_empty(rb)) 
    {
        return false;  // 队列为空，无法取出数据
    }

    // 读取数据包的长度 (2 bytes)
    *len = (rb->buffer[rb->head] << 8) | rb->buffer[(rb->head + 1) % BUFFER_SIZE];
    rb->head = (rb->head + 2) % BUFFER_SIZE;  // 更新队列头，防止越界

    // 读取数据包内容
    for (uint16_t i = 0; i < *len; i++) 
    {
        data[i] = rb->buffer[rb->head];
        rb->head = (rb->head + 1) % BUFFER_SIZE;  // 每次都要防止越界
    }

    rb->size -= (*len + 2);  // 更新队列中数据的总大小
    return true;
}
