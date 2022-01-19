typedef struct Router {
  char ip[20];
  pid_t id;
  pid_t pid;
  pid_t parent_pid;
  int port;
  int buffer_length;
  struct Neighbour *neighbours[3];
} Router;
