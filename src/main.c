// printf
#include "stdio.h"
// atoi
#include "stdlib.h"
// pid_t
#include "sys/types.h"
// getpid()
#include "unistd.h"

// My custom functions
#include "load_config.h"

// Configuration files path
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

typedef struct router {
  pid_t id;
  pid_t process_id;
  pid_t parent_id;
  int port;
  char ip[20];
} Router;

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("Router Id expected !!!\n");
    return -1;
  }

  Router *r1 = malloc(sizeof(Router));
  // Id passed via argument on execution
  r1->id = atoi(argv[1]);
  // Get own process id
  r1->process_id = getpid();
  // Get parent process id
  r1->parent_id = getppid();


  // set port and ip
  parse_router_config(roteador_cfg, r1->id, &r1->port, r1->ip);

  printf("Router  id: %10d\n", r1->id);
  printf("Process id: %10d\n", r1->process_id);
  printf("Parent pid: %10d\n", r1->parent_id);
  printf("Port      : %10d\n", r1->port);
  printf("Ip addr   :  %s\n",  r1->ip);


  // parse_file(enlaces_cfg, r1->id);
  // parse_file(roteador_cfg, r1->id);

  printf("\n*** Fim do programa ***\n");
  return 0;
};
