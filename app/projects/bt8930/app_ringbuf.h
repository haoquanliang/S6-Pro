#ifndef _APP_RINGBUF_H_
#define _APP_RINGBUF_H_

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 300   // 缓冲区大小

// 定义环形缓冲区结构
typedef struct {
    uint8_t buffer[BUFFER_SIZE];  // 存储数据包
    uint16_t head;                // 队列头，指向数据包的开始
    uint16_t tail;                // 队列尾，指向下一个可用位置
    uint16_t size;                // 当前队列中存储的数据总大小
} ring_buffer;

void app_ring_buffer_init(ring_buffer *rb) ;
bool app_ring_buffer_is_full(ring_buffer *rb, uint16_t data_len);
bool app_ring_buffer_is_empty(ring_buffer *rb);
bool app_ring_buffer_enqueue(ring_buffer *rb, uint8_t *data, uint16_t len);
bool app_ring_buffer_dequeue(ring_buffer *rb, uint8_t *data, uint16_t *len) ;

#endif 
