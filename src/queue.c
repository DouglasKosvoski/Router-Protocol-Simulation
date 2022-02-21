/*
* queue.c
*/

#include "queue.h"

// Set queue default values (need to be called manually)
void queue_init(Queue *q) {
  q->item_count = 0;
  q->tail = -1;
  q->head = 0;
}

// Retrieve first element from the queue
char* queue_start(Queue *q) {
  return q->queue[q->head];
}

// Check if there is anything on it, returns True if empty and False otherwise
bool queue_is_empty(Queue *q) {
  return q->item_count == 0;
}

// Check if queue have any slot avaiable, returns True if full and False otherwise
bool queue_is_full(Queue *q) {
  return q->item_count == CAPACITY;
}

// Retrive how many element are currently on queue
int queue_size(Queue *q) {
  return q->item_count;
}

// Insert element at the end of queue, if any slot are avaiable
void queue_insert(Queue *q, char data[]) {
  if(!queue_is_full(q)) {
    if(q->tail == CAPACITY-1) {
      q->tail = -1;
    }
    strcpy(q->queue[++q->tail], data);
    q->item_count++;
  }
}

// Remove first element from queue and add one to avaiable slots
void queue_remove(Queue *q) {
  q->head++;
  if(q->head == CAPACITY) {
    q->head = 0;
  } q->item_count--;
}

// Loop through queue and display its elements
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

