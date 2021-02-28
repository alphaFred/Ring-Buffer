#include "ringbuffer.h"

/**
 * @file
 * Implementation of ring buffer functions.
 */

void ring_buffer_init(ring_buffer_t *buffer) {
  buffer->tail_index = 0;
  buffer->head_index = 0;
}

void ring_buffer_queue(ring_buffer_t *buffer, char data) {
  /* Is buffer full? */
  if(ring_buffer_is_full(buffer)) {
    /* Is going to overwrite the oldest byte */
    /* Increase tail index */
    buffer->tail_index = ((buffer->tail_index + 1) & RING_BUFFER_MASK);
  }

  /* Place data in buffer */
  buffer->buffer[buffer->head_index] = data;
  buffer->head_index = ((buffer->head_index + 1) & RING_BUFFER_MASK);
}

void ring_buffer_queue_arr(ring_buffer_t *buffer, const char *data, ring_buffer_size_t size) {
  /* Add bytes; one by one */
  ring_buffer_size_t i;
  for(i = 0; i < size; i++) {
    ring_buffer_queue(buffer, data[i]);
  }
}

uint8_t ring_buffer_dequeue(ring_buffer_t *buffer, char *data) {
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0;
  }
  
  *data = buffer->buffer[buffer->tail_index];
  buffer->tail_index = ((buffer->tail_index + 1) & RING_BUFFER_MASK);
  return 1;
}

ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, char *data, ring_buffer_size_t len) {
  if(ring_buffer_is_empty(buffer)) {
    /* No items */
    return 0;
  }

  char *data_ptr = data;
  ring_buffer_size_t cnt = 0;
  while((cnt < len) && ring_buffer_dequeue(buffer, data_ptr)) {
    cnt++;
    data_ptr++;
  }
  return cnt;
}

uint8_t ring_buffer_peek(ring_buffer_t *buffer, char *data, ring_buffer_size_t index) {
  if(index >= ring_buffer_num_items(buffer)) {
    /* No items at index */
    return 0;
  }
  
  /* Add index to pointer */
  ring_buffer_size_t data_index = ((buffer->tail_index + index) & RING_BUFFER_MASK);
  *data = buffer->buffer[data_index];
  return 1;
}

uint8_t ring_buffer_search(ring_buffer_t *buffer, char search_data, ring_buffer_size_t* search_idx)
{
    char data;

    if(ring_buffer_is_empty(buffer))
    {
        return 0;
    }

    for (int idx = (ring_buffer_num_items(buffer) - 1); idx >= 0; --idx)
    {
        ring_buffer_peek(buffer, &data, idx);
        if(data == search_data)
        {
            *search_idx = idx;
            return 1;
        }
    }
    return 0;
}

uint8_t ring_buffer_match(ring_buffer_t* search_buffer, ring_buffer_t* match_buffer, ring_buffer_size_t* match_idx, ring_buffer_size_t* match_len)
{
    char search_data = ' ';
    char match_data = ' ';
    char start_match_data = ' ';
    ring_buffer_size_t i = 0U;
    ring_buffer_size_t j = 0U;
    ring_buffer_size_t _match_idx = 0U;
    ring_buffer_size_t _match_len = 0U;
    ring_buffer_size_t search_buffer_len = ring_buffer_num_items(search_buffer);
    ring_buffer_size_t match_buffer_len = ring_buffer_num_items(match_buffer);

    ring_buffer_peek(match_buffer, &start_match_data, 0);

    for (i = 0; i < search_buffer_len; ++i) {
        ring_buffer_peek(search_buffer, &search_data, i);
        if(start_match_data == search_data)
        {
            ring_buffer_size_t max_j = (((search_buffer_len - i) < match_buffer_len) ? (search_buffer_len - i) : match_buffer_len);
            for (j = 1; j <= max_j; ++j) {
                ring_buffer_peek(match_buffer, &match_data, j);
                ring_buffer_peek(search_buffer, &search_data, (i + j));
                if(match_data != search_data)
                {
                    break;
                }
            }
            if(j > _match_len)
            {
                _match_len = j;
                _match_idx = i;
            }
        }
    }

    *match_idx = _match_idx;
    *match_len = _match_len;
    return 0;
}

extern inline uint8_t ring_buffer_is_empty(ring_buffer_t *buffer);
extern inline uint8_t ring_buffer_is_full(ring_buffer_t *buffer);
extern inline ring_buffer_size_t ring_buffer_num_items(ring_buffer_t *buffer);

