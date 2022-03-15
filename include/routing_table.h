/*
 * routing_table.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NHOP 1
#define COST 0
#define INF 99
#define SIZE 6

typedef struct Routing_table
{
  int routes[SIZE + 1][2];
} Routing_table;

// Initialize default values to Routing Table (need to be called manually)
void init_routing_table(Routing_table *rt);

// Get self distance vector and serialize to string
void serialize_dt(Routing_table *rt, char *str);

// Display avaiable routes with cost and next hop
void display_routing_table(Routing_table *rt);

// Set element on distance vector
void routing_table_set(Routing_table *rt, int id, int cost, int next_hop_id);

// Check and compare received distance vector from neighbours to self distance vector
// if there is a route 'cheaper' than current route update it to new
// 'return' does the trick, if True updated distance vector will be sent to neighbours
int bellman_ford_distributed(Routing_table *rt, char *content, int next_hop);