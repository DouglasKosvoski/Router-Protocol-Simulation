// printf
#include "stdio.h"
// atoi
#include "stdlib.h"
// pid_t
#include "sys/types.h"
// getpid()
#include "unistd.h"

// My custom functions
#include "neighbour.h"
#include "router.h"
#include "load_config.h"

// Configuration files path
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

// Check if router id was passed on execution
int args_passed(int args) {
  if (args < 2) {
    printf("Router Id expected !!!\n");
    return -1;
  }
  return 0;
}

// Display information about the router
void display_router_info(Router *r) {
  printf("\n----------- INFO -----------\n");
  printf("Router  id   : %13d \n", r->id);
  printf("Process id   : %13d \n", r->process_id);
  printf("Parent pid   : %13d \n", r->parent_id);
  printf("Port         : %13d \n", r->port);
  printf("IP addr      :     %s",  r->ip);
  printf("----------------------------\n\n");
}

// Parse router config from cfg
int set_router_data(Router *r, const char *i) {
  // Id passed via argument on execution
  r->id = atoi(i);
  // Get own process id
  r->process_id = getpid();
  // Get parent process id
  r->parent_id = getppid();

  // set port and ip
  return parse_router_config(roteador_cfg, r->id, &r->port, r->ip);
}

int main(int argc, char const *argv[]) {
  // check if id was passed as argument
  if (args_passed(argc) == -1) return -1;

  Router *r1 = malloc(sizeof(Router));

  if (set_router_data(r1, argv[1]) == -1) return -1;

  display_router_info(r1);

  ////////////////
  char s[20];
  parse_enlaces_config(enlaces_cfg, r1->id, s);
  char temp[20]; strcpy(temp, s);
  char *token = strtok(temp, " ");

  int counter = 0;
  while(token != NULL) {
    counter++;
    token = strtok(NULL, " ");
  }

  Neighbour *n1 = malloc(sizeof(Neighbour));
  Neighbour *n2 = malloc(sizeof(Neighbour));
  Neighbour *n3 = malloc(sizeof(Neighbour));
  r1->neighbours[0] = n1;
  r1->neighbours[1] = n2;
  r1->neighbours[2] = n3;
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);
  printf("%d %d %d %d %d %d\n", n1->id, n1->cost, n2->id, n2->cost, n3->id, n3->cost);

  printf("%d\n", r1->neighbours[0]->id);
  printf("c:%d\n", r1->neighbours[0]->cost);
  printf("%d\n", r1->neighbours[1]->id);
  printf("c:%d\n", r1->neighbours[1]->cost);
  printf("%d\n", r1->neighbours[2]->id);
  printf("c:%d\n", r1->neighbours[2]->cost);

  //////////////

  // get_neighbours_data(enlaces_cfg, r1->id);

  // // Create threads
  // pthread_create(&Thread1, NULL, server, NULL);
  // pthread_create(&Thread2, NULL, client, NULL);
  //
  // // Join threads
  // pthread_join(Thread1, NULL);
  // printf("Thread ID: %ld returned\n", Thread1);
  // pthread_join(Thread2, NULL);
  // printf("Thread ID: %ld returned\n", Thread2);

  printf("\n*** Fim do programa ***\n");
  return 0;
};
