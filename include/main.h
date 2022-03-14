/*
* main.h
*/

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#include "routing_table.h"
#include "utils.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"
#include "queue.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

// sleep time between each distance vector sharing
#define RT_INTERVAL 30

pthread_mutex_t in_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t out_mutex = PTHREAD_MUTEX_INITIALIZER;
Queue *q_in, *q_out, *user_messages_received;
Routing_table *rt; Router *r1;
sem_t sem_in, sem_out;

// Send distance vector to all attached neighbours
void send_distance_vector();

// Parse router config from cfg
int set_router(const char *i);

// Display information about the router
void display_router_info();

// Loop over all neighbours and display useful info about 'em (and itself)
void display_neighbours_info();

// Get all attributes from Message and concatenate into a single string
void serialize_message(char *m, Message *msg);

// Parse string into Message object
void deserialize_msg(Message *msg, char *serialized_msg);

// Get user msg input from terminal
void get_user_message();

void display_received_messages();

// Terminal thread, display menu, get user input and redirect to chosen option
void *terminal(void *);

// Manage all incoming data
void *packet_handler(void *);

// Sender Thread keep on listening to outgoing queue and send msg when there is anything
void *sender(void *);

// Receiver thread, is always linstening to incoming data on specified port
void *receiver(void *);

void *send_rt_periodically(void *);