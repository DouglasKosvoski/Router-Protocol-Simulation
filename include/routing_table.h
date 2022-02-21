/*
* routing_table.h
*/

#include <stdio.h>      // input/output
#include <stdlib.h>     // data convertions
#include <string.h>     // string manipulation

#define NHOP 1
#define COST 0
#define INF 99
#define SIZE 6

typedef struct Routing_table {
  int routes[SIZE+1][2];  
} Routing_table;

void init_routing_table(Routing_table *rt);
void sply(Routing_table *rt, char *str);
void display_routing_table(Routing_table *rt);
void routing_table_set(Routing_table *rt, int id, int cost, int next_hop_id);
int update_distance_vector(Routing_table *rt, char *content, int next_hop);