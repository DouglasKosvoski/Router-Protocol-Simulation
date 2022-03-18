/*
 * main.h
 */

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "routing_table.h"
#include "utils.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"
#include "queue.h"

// enlace file configuration path
#define enlaces_cfg "./cfg/enlaces.config"
// router file configuration path
#define roteador_cfg "./cfg/roteador.config"

// sleep time between each distance vector routine
#define RT_INTERVAL 30

// mutexes to control incoming and outgoing Queues
pthread_mutex_t in_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t out_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ack_mutex = PTHREAD_MUTEX_INITIALIZER;
// semaphores to control Queue state
sem_t sem_in, sem_out;
// incoming and outgoing Queues, aswell as Queue to store user data
Queue *q_in, *q_out, *q_user_msgs, *q_ack;
// router routing table
Routing_table *rt;
// self Router
Router *r1;

/* Function Prototypes */

int main(int argc, char const *argv[]);

// Send distance vector to all attached neighbours
void send_distance_vector();

// Parse router config from cfg
int set_router(const char *i);

// Display information about the router
void display_router_info();

// Loop over all neighbours and display useful info about 'em (and itself);

void display_reachable_routers();

// Get all attributes from Message and concatenate into a single string
void serialize_message(char *m, Message *msg);

// Parse string into Message object
void deserialize_msg(Message *msg, char *serialized_msg);

// Get user msg input from terminal
void get_user_message();

// Loops through a Queue and display its content nicely formatted
void display_received_messages();

// Terminal thread, display menu, get user input and redirect to chosen option
void *terminal(void *arg);

// Manage all incoming data
void *packet_handler(void *arg);

// Sender Thread keep on listening to outgoing queue and send msg when there is anything
void *sender(void *arg);

// Receiver thread, is always linstening to incoming data on specified port
void *receiver(void *arg);

void *rt_routine(void *arg);

void *ack_routine(void *arg);