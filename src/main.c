#include "stdio.h"
#include "stdlib.h"

// My custom functions
#include "load_config.h"

// Configuration files path
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

int main(int argc, char const *argv[]) {
  parse_file(enlaces_cfg);
  parse_file(roteador_cfg);
  printf("\n*** Fim do programa ***\n");
  return 0;
};
