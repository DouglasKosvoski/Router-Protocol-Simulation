typedef struct Router {
  pid_t id;
  pid_t process_id;
  pid_t parent_id;
  int port;
  char ip[20];
  struct Neighbour *neighbours[3];
} Router;
