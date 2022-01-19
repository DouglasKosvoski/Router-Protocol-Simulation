typedef struct Router {
  char ip[20];
  pid_t id;
  pid_t pid;
  pid_t parent_pid;
  unsigned int port;
  unsigned short buffer_length;
  struct Neighbour *neighbours[3];
} Router;
