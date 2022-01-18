/*
* Author `Douglas Kosvoski`
* Class `Redes de Computadores 2021.2`
*
* Router protocol simulation via UDP sockets
* and Distributed Bellman-Ford Algorithm
*
* 2022/1
*/

/* Includes */
#include "stdio.h"      // input/output
#include "stdlib.h"     // data convertions
#include "sys/types.h"  // data types
#include "unistd.h"     // constant and types
#include "arpa/inet.h"  // internet operations
#include "sys/socket.h" // sockets definitions
#include "pthread.h"    // thread manipulation

/* Custom structures and functions*/
#include "neighbour.h"
#include "router.h"
#include "load_config.h"
#include "message.h"
#include "server.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

/* Declarations */
void check_arguments(int args);
void display_router_info(Router *r);
void set_router(Router *r, const char *i);
void set_neighbours(Router *r);

/* Main */
int main(int argc, char const *argv[]) {
  // check if id was passed as argument
  check_arguments(argc);

  // allocate router and threads
  Router *r1 = malloc(sizeof(Router));

  // set buffer
  set_router(r1, argv[1]);
  // print some info onto the terminal, id, ip, port, pid
  display_router_info(r1);

  // Create threads
  pthread_t Thread1, Thread2, Thread3, Thread4;
  pthread_create(&Thread1, NULL, receiver, r1);
  // Join threads
  pthread_join(Thread1, NULL);
  printf("Thread ID: %ld returned\n", Thread1);

  printf("\n*** Fim do programa ***\n");
  exit(0);
};

/* Functions */

// Check if router id was passed on execution
void check_arguments(int args) {
  if (args < 2) {
    printf("\nRouter Id expected !!!\n");
    printf("Ex: `./router 4` \n\n");
    exit(1);
  }
}

// Parse router config from cfg
void set_router(Router *r, const char *i) {
  // id passed via argument on execution, convert to int
  r->id = atoi(i);
  // get own process id
  r->pid = getpid();
  // get parent process id
  r->parent_pid = getppid();
  // set max buffer lenght for transfer
  r->buffer_length = 512;
  // set port and ip
  parse_router_config(roteador_cfg, r->id, &r->port, r->ip);
  // allocate and attach neighbours to router
  set_neighbours(r);
}

// Allocate and attach neighbours to router
void set_neighbours(Router *r) {
  // if router does not have 3 neighbours
  // unused Neighbour will be set to id=0 and cost=0
  char s[20];
  parse_enlaces_config(enlaces_cfg, r->id, s);

  // alloc Neighbours in memory
  Neighbour *n1 = malloc(sizeof(Neighbour));
  Neighbour *n2 = malloc(sizeof(Neighbour));
  Neighbour *n3 = malloc(sizeof(Neighbour));
  // attach neighbours to router
  r->neighbours[0] = n1; r->neighbours[1] = n2; r->neighbours[2] = n3;
  // set neighbours data
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);
}

// Display information about the router
void display_router_info(Router *r) {
  printf("\n----------- INFO -----------\n");
  printf("Router  id   : %13d \n", r->id);
  printf("Process id   : %13d \n", r->pid);
  printf("Parent pid   : %13d \n", r->parent_pid);
  printf("Buffer len   : %13d \n", r->buffer_length);
  printf("Port         : %13d \n", r->port);
  printf("IP addr      :     %s",  r->ip);
  printf("----------------------------\n\n");
}
