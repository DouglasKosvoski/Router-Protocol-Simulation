/*
* queue.h
*/

#include <stdio.h>      // input/output
#include <string.h>     // string manipulation
#include <stdbool.h>

#define CAPACITY 3
#define ITEM_SIZE 1024

typedef struct Queue {
  int head;
  // remember to set tail = -1;
  int tail;
  int item_count;
  char queue[CAPACITY][ITEM_SIZE];
} Queue;

char* queue_start(Queue *q);
bool queue_is_empty(Queue *q);
bool queue_is_full(Queue *q);
int queue_size(Queue *q);
void queue_insert(Queue *q, char data[]);
void queue_remove(Queue *q);
void display_queue_content(Queue *q);