#include <stdio.h>      // input/output
#include <string.h>     // string manipulation
#include <stdbool.h>

#include "queue.h"

char* queue_start(Queue *q) {
  return q->queue[q->head];
}

bool queue_is_empty(Queue *q) {
  return q->item_count == 0;
}

bool queue_is_full(Queue *q) {
  return q->item_count == CAPACITY;
}

int queue_size(Queue *q) {
  return q->item_count;
}

void queue_insert(Queue *q, char data[]) {
  if(!queue_is_full(q)) {
    if(q->tail == CAPACITY-1) {
      q->tail = -1;
    }
    strcpy(q->queue[++q->tail], data);
    q->item_count++;
  }
  else {
  }
}

void queue_remove(Queue *q) {
  q->head++;
  if(q->head == CAPACITY) {
    q->head = 0;
  } q->item_count--;
}

void display_queue_content(Queue *q) {
  if (queue_size(q) == 0) {
    printf("\n ----------------------------");
    printf("\n -> Queue is Empty \n");
    printf(" ----------------------------\n");
    return;
  }
  for (size_t i = 0; i < CAPACITY; i++) {
    printf("(%ld) | %s\n", i, q->queue[i]);
  }
}

