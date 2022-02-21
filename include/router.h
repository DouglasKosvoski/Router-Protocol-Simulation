/*
* router.h
*/

#include <stdlib.h>

typedef struct Router {
  // router id
  pid_t id;
  // process id
  pid_t pid;
  // parent process id
  pid_t parent_pid;
  // router ip address
  char ip[20];
  // comunication port
  int port;
  // max buffer size
  int buffer_length;
  // router directly connecteds
  struct Neighbour *neighbours[3];
} Router;
