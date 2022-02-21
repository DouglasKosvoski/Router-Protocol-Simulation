/*
* main.h
*/

#include <unistd.h>     // constant and types
#include <arpa/inet.h>  // internet operations
#include <pthread.h>    // thread manipulation
#include <stdbool.h>

#include "routing_table.h"
#include "load_config.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"
#include "queue.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

pthread_mutex_t in_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t out_mutex = PTHREAD_MUTEX_INITIALIZER;
Routing_table *rt; Router *r1;
Queue *q_in; Queue *q_out;

void send_distance_vector();
void set_router(const char *i);
void display_router_info();
void display_neighbours_info();
void serialize_message(char *m, Message *msg);
void deserialize_msg(Message *msg, char *serialized_msg);
void get_user_message();
void *terminal(void *);
void *packet_handler(void *);
void *sender(void *);
void *receiver(void *);