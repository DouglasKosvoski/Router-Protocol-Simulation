#define NHOP 1
#define COST 0
#define INF 99
#define SIZE 6

typedef struct Routing_table {
  int routes[SIZE][2];  
} Routing_table;

void init_routing_table(Routing_table *rt) {
  for (size_t i = 0; i < SIZE; i++) {
    rt->routes[i][0] = INF;
    rt->routes[i][1] = -1;
  }
}

void sply(Routing_table *rt, char *str) {
  char values[256], temp[10], separator[2] = "^";
  memset(values, 0, strlen(values));
  
  for (size_t i = 0; i < SIZE; i++) {
    sprintf(temp, "%d", rt->routes[i][0]);
    strncat(values, temp, strlen(temp));
    strncat(values, separator, strlen(separator));
    sprintf(temp, "%d", rt->routes[i][1]);
    strncat(values, temp, strlen(temp));
    strncat(values, separator, strlen(separator));
  }
  strcpy(str, values);
}

void display_routing_table(Routing_table *rt) {
  // print header
  printf("\n Routing Table: \n       ");
  for (size_t i = 0; i < SIZE; i++) {
    printf("%3d ", i+1);
  } printf("\n");
  printf("       ------------------------\n");
  
  printf(" cost |");
  for (size_t i = 0; i < SIZE; i++) {
    printf("%3d ", rt->routes[i][0]);
  } printf("| \n");
  
  printf(" nHop |");
  for (size_t i = 0; i < SIZE; i++) {
    printf("%3d ", rt->routes[i][1]);
  } printf("| \n");

  printf("       ------------------------\n");
}

void routing_table_set(Routing_table *rt, int id, int cost, int next_hop_id) {
  rt->routes[id-1][COST] = cost;
  rt->routes[id-1][NHOP] = next_hop_id;
}
