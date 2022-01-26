typedef struct Queue {
  int start;
  int end;
  int count;
  Message q[10];
} Queue;

void init(Queue *queue) {
  queue->count = 0;
  queue->start = 0;
  queue->end = 10;
}

void add_to_queue(Queue *queue, Message *m) {
  queue->q[queue->count++] = *m;
}

int queue_size(Queue *queue) {
  return queue->count;
}
