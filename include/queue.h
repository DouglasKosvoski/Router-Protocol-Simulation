/*
 * queue.h
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// max Queue size, how many objects it can store
#define CAPACITY 15
// max item size an object can have
#define ITEM_SIZE 1024

typedef struct Queue
{
  // start of the Queue
  int head;
  // end of the Queue
  int tail;
  // how many items stored
  int item_count;
  // Queue storage containing the content
  char queue[CAPACITY][ITEM_SIZE];
} Queue;

// Set queue default values (need to be called manually)
void queue_init(Queue *q);

// Retrieve first element from the queue
char *queue_start(Queue *q);

// Check if there is anything on it, returns True if empty and False otherwise
bool queue_is_empty(Queue *q);

// Check if queue have any slot avaiable, returns True if full and False otherwise
bool queue_is_full(Queue *q);

// Retrive how many element are currently on queue
int queue_size(Queue *q);

// Insert element at the end of queue, if any slot are avaiable
void queue_insert(Queue *q, char data[]);

// Remove first element from queue and add one to avaiable slots
void queue_remove(Queue *q);

// Loop through queue and display its elements
void display_queue_content(Queue *q);
