/*
 * routing_table.c
 */

#include "routing_table.h"

// Initialize default values to Routing Table (need to be called manually)
void init_routing_table(Routing_table *rt)
{
  for (size_t i = 1; i < SIZE + 1; i++)
  {
    rt->routes[i][0] = INF;
    rt->routes[i][1] = -1;
    rt->original[i][0] = INF;
    rt->original[i][1] = -1;
  }
}

// Get self distance vector and serialize to string
void serialize_rt(Routing_table *rt, char *str)
{
  char values[256] = "", temp[10] = "", separator[2] = "*";
  memset(values, 0, strlen(values));

  for (size_t i = 1; i < SIZE + 1; i++)
  {
    sprintf(temp, "%d", rt->routes[i][0]);
    strncat(values, temp, strlen(temp));
    strncat(values, separator, strlen(separator));
  }
  strcpy(str, values);
}

// Display avaiable routes with cost and next hop
void display_routing_table(Routing_table *rt)
{
  // print header
  printf("\n Routing Table: \n       ");
  for (size_t i = 0; i < SIZE; i++)
  {
    printf("%3ld ", i + 1);
  }
  printf("\n");
  printf("       ------------------------\n");

  printf(" cost |");
  for (size_t i = 1; i < SIZE + 1; i++)
  {
    printf("%3d ", rt->routes[i][0]);
  }
  printf("| \n");

  printf(" nHop |");
  for (size_t i = 1; i < SIZE + 1; i++)
  {
    printf("%3d ", rt->routes[i][1]);
  }
  printf("| \n");

  printf("       ------------------------\n");
}

// Set element on distance vector
void routing_table_set(Routing_table *rt, int id, int cost, int next_hop_id, int ori)
{
  if (ori == 0)
  {
    rt->original[id][COST] = cost;
    rt->original[id][NHOP] = next_hop_id;
  }
  else
  {
    rt->routes[id][COST] = cost;
    rt->routes[id][NHOP] = next_hop_id;
  }
}

// Check and compare received distance vector from neighbours to self distance vector
// if there is a route 'cheaper' than current route update it to new
// 'return' does the trick, if True updated distance vector will be sent to neighbours
int bellman_ford_distributed(Routing_table *rt, char *content, int next_hop, int my_id)
{
  char before[256], after[256];
  serialize_rt(rt, before);

  // printf("\n Running B-F on `%s` from %d\n", content, next_hop);
  char delim[2] = "*", *token = strtok(content, delim);
  int new_cost, i = 1, cur_cost = 99, table_updated = 0;

  // loop through received distance vector
  while (token != NULL)
  {
    cur_cost = rt->routes[i][COST];
    new_cost = atoi(token);

    // if current cost is not infinite, then add enlace cost
    if (rt->routes[next_hop][COST] != INF)
    {
      new_cost += rt->original[next_hop][COST];
    }

    if (new_cost == 0 && i != my_id)
    {
      return 0;
    }

    // printf(" (%d) | atual:%d novo:%d  ori:%d\n", i, cur_cost, new_cost, next_hop);
    // update internal distance vector
    if (new_cost < cur_cost || rt->routes[i][NHOP] == next_hop)
    {
      // printf("melhor encontrado\n");
      if (new_cost > 50)
      {
        routing_table_set(rt, i, INF, -1, 1);
      }
      else
      {
        routing_table_set(rt, i, new_cost, next_hop, 1);
      }
    }
    i++;
    token = strtok(NULL, delim);
  }

  // if true, will propagate new distance vector to the others neighbours
  serialize_rt(rt, after);
  table_updated = strcmp(before, after) == 0 ? 0 : 1;
  return table_updated;
}