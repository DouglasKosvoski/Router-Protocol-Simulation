#define DESTINATION 0
#define NEXT_HOP 1
#define COST 2

#define SIZE 7

typedef struct Routing_table {
  int routes[SIZE][SIZE];
  
} Routing_table;

void display_routing_table(Routing_table *rt) {
  // print header
  printf("\n Routing Table: "); printf("\n\n     ");
  for (size_t i = 1; i < SIZE; i++) printf("  %d ", i); printf("\n");
  
  printf("    --------------------------\n");
  for (size_t i = 1; i < SIZE; i++) {
    printf(" %d | ", i);
    for (size_t j = 1; j < SIZE; j++) {
      printf("%3d ", rt->routes[i][j]);
    }
    printf("|\n");
  }
  printf("    --------------------------\n");
}

int *vrasy(Routing_table *rt, int id) {
  return rt->routes[id];
}