/*
* neighbour.h
*/

#include <stdlib.h>     // data convertions

typedef struct Neighbour {
  // router id
  pid_t id;
  // cost representing weighted graph
  int cost;
  // router port
  int port;
  // router ip address
  char ip[20];
} Neighbour;
