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

int main(int argc, char const *argv[]) {

  // Id passed via argument on execution
  pid_t rot_id = atoi(argv[1]);
  // Get own process id
  pid_t process_id = getpid();
  // Get parent process id
  pid_t p_process_id = getppid();

  printf("Router  id: %5d\n", rot_id);
  printf("Process id: %5d\n", process_id);
  printf("Parent pid: %5d\n", p_process_id);


  parse_file(enlaces_cfg);
  parse_file(roteador_cfg);

  printf("\n*** Fim do programa ***\n");
  return 0;
};
