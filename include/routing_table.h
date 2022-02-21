#define NHOP 1
#define COST 0
#define INF 99
#define SIZE 6

typedef struct Routing_table {
  int routes[SIZE+1][2];  
} Routing_table;

void init_routing_table(Routing_table *rt) {
  for (size_t i = 1; i < SIZE+1; i++) {
    rt->routes[i][0] = INF;
    rt->routes[i][1] = -1;
  }
}

void sply(Routing_table *rt, char *str) {
  char values[256], temp[10], separator[2] = "*";
  memset(values, 0, strlen(values));
  
  for (size_t i = 1; i < SIZE+1; i++) {
    sprintf(temp, "%d", rt->routes[i][0]);
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
  for (size_t i = 1; i < SIZE+1; i++) {
    printf("%3d ", rt->routes[i][0]);
  } printf("| \n");
  
  printf(" nHop |");
  for (size_t i = 1; i < SIZE+1; i++) {
    printf("%3d ", rt->routes[i][1]);
  } printf("| \n");

  printf("       ------------------------\n");
}

void routing_table_set(Routing_table *rt, int id, int cost, int next_hop_id) {
  rt->routes[id][COST] = cost;
  rt->routes[id][NHOP] = next_hop_id;
}

int update_distance_vector(Routing_table *rt, char *content, int next_hop) {
  char delim[2] = "*", *token = strtok(content, delim);
  int new_cost = -1, i = 1, cur_cost = 99, cur_nhop = -1, table_updated = 0;

  while(token != NULL) {
    cur_cost = rt->routes[i][COST];
    cur_nhop = rt->routes[i][NHOP];
    new_cost = atoi(token);

    // if current cost is not infinite, then add enlace cost
    if (rt->routes[next_hop][COST] != INF) {
      new_cost += rt->routes[next_hop][COST];
    }

    if (new_cost < cur_cost) {
      // update internal distance vector
      routing_table_set(rt, i, new_cost, next_hop);
      table_updated = 1;
    }
    i++; token = strtok(NULL, delim);
  }
  // if true, will propagate to the others neighbours
  return table_updated;
}