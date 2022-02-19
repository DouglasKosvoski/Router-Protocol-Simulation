/*
* Author `Douglas Kosvoski`
* Class `Redes de Computadores 2021.2`
*
* Router protocol simulation via UDP sockets
* and Distributed Bellman-Ford Algorithm
*
* 2022/1
*/

/* Includes */
#include <stdio.h>      // input/output
#include <stdlib.h>     // data convertions
#include <sys/types.h>  // data types
#include <unistd.h>     // constant and types
#include <arpa/inet.h>  // internet operations
#include <sys/socket.h> // sockets definitions
#include <pthread.h>    // thread manipulation
#include <string.h>     // string manipulation
#include <stdbool.h>

/* Custom structures and functions */
#include "load_config.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

/* Functions */
void check_arguments(int args);
void set_router(Router *r, const char *i);
void display_router_info(Router *r);
void *sender(void *data);
void *receiver(void *data);
void *terminal(void *data);
void *packet_handler();
void display_neighbours_info(Router *r);
void clear_terminal();

/* QUEUE */
#define MAX 3
char queue[MAX][128];
int queue_head = 0, queue_tail = -1, queue_item_count = 0;

char* queue_start() { return queue[queue_head]; }

bool queue_is_empty() { return queue_item_count == 0; }

bool queue_is_full() { return queue_item_count == MAX; }

int queue_size() { return queue_item_count; }

void queue_insert(char data[]) {
  if(!queue_is_full()) {
    if(queue_tail == MAX-1) { queue_tail = -1; }
    strcpy(queue[++queue_tail], data);
    queue_item_count++;
  }
}

void queue_remove() {
  if(queue_head++ == MAX) {
    queue_head = 0;
  } queue_item_count--;
}

void display_received_msgs() {
  if (queue_size() == 0) {
    clear_terminal();
    printf("\n ----------------------------");
    printf("\n -> Queue is Empty \n");
    printf(" ----------------------------\n");
    return;
  }
  for (size_t i = 0; i < MAX; i++) {
    printf("(%ld) | %s\n", i, queue[i]);
  }
}
/* END QUEUE */

/* Main */
int main(int argc, char const *argv[]) {
  // check if id was passed as argument
  check_arguments(argc);

  // allocate router
  Router *r1 = malloc(sizeof(Router));
  // set buffer
  set_router(r1, argv[1]);
  // print some info onto the terminal, id, ip, port, pid
  display_router_info(r1);

  // Create threads
  pthread_t th_receiver, th_terminal;
  pthread_create(&th_receiver, NULL, (void *)receiver, r1);
  pthread_create(&th_terminal, NULL, (void *)terminal, r1);

  // Join threads
  pthread_join(th_receiver, NULL);
  pthread_join(th_terminal, NULL);
  printf("Thread ID: %ld returned\n", th_receiver);
  printf("Thread ID: %ld returned\n", th_terminal);

  printf("\n*** Fim do programa ***\n");
  exit(0);
};

void clear_terminal() { system("clear"); }

// Check if router id was passed on execution
void check_arguments(int args) {
  if (args < 2) {
    printf("\nRouter Id expected !!!\n");
    printf("Ex: `./router 1` \n\n");
    exit(1);
  }
}

// Parse router config from cfg
void set_router(Router *r, const char *i) {
  // id passed via argument on execution, convert to int
  r->id = atoi(i);
  // get own process id
  r->pid = getpid();
  // get parent process id
  r->parent_pid = getppid();
  // set max buffer length for transfer
  r->buffer_length = 1024;

  // set port and ip
  if (parse_router_config(roteador_cfg, r->id, &r->port, r->ip) == -1) {
    printf("Router not Found!\n");
    exit(-1);
  }

  // Allocate and attach neighbours to router
  char s[20]; parse_enlaces_config(enlaces_cfg, r->id, s);
  // alloc Neighbours in memory
  Neighbour *n1 = malloc(sizeof(Neighbour));
  Neighbour *n2 = malloc(sizeof(Neighbour));
  Neighbour *n3 = malloc(sizeof(Neighbour));
  r->neighbours[0] = n1; r->neighbours[1] = n2; r->neighbours[2] = n3;
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);

  // Set ip and ports
  for (size_t i = 0; i < sizeof(r->neighbours) / sizeof(r->neighbours[0]); i++) {
    if (parse_router_config(roteador_cfg, r->neighbours[i]->id, &r->neighbours[i]->port, r->neighbours[i]->ip) == -1) {
      continue;
    }
  }
}

// Display information about the router
void display_router_info(Router *r) {
  clear_terminal();
  printf("\n ----------- INFO -----------\n");
  printf(" Router  id   : %13d \n", r->id);
  printf(" Process id   : %13d \n", r->pid);
  printf(" Parent pid   : %13d \n", r->parent_pid);
  printf(" Buffer len   : %13d \n", r->buffer_length);
  printf(" Port         : %13d \n", r->port);
  printf(" IP addr      :     %s\n",  r->ip);
  printf(" ----------------------------\n\n");
}

// Loop over all neighbours and display useful info about 'em (and itself)
void display_neighbours_info(Router *r) {
  clear_terminal(); printf("\n Neighbours Information");
  printf("\n  ------------------------------------------\n");
  printf(" | index | id \t| cost \t| port \t|     ip    |");
  printf("\n  -------------------------------------------\n");
  printf(" (self)");
  printf("\n  ------------------------------------------\n");
  printf(" |  %d   | %d \t| %d \t| %d | %s | \n", -1, r->id, 0, r->port, r->ip);
  printf("  ------------------------------------------\n");
  printf(" (neighbours)\n");
  printf("  ------------------------------------------\n");
  
  // Loop over neighbours and print their info
  for (size_t i = 0; i < sizeof(r->neighbours) / sizeof(r->neighbours[0]); i++) {
    if (r->neighbours[i]->cost > 15) { continue; };
    printf(" |   %d \t | %d \t| %d \t| %d | %s | \n", i, r->neighbours[i]->id, r->neighbours[i]->cost, r->neighbours[i]->port, r->neighbours[i]->ip);
  } printf("  ------------------------------------------\n");
}

// Get all attributes from Message and concatenate into a single string
void serialize_message(char *m, Message *msg) {
  char serialized_msg[1024] = "";
  char separator[2] = "^";
  char temp[20];
  
  sprintf(serialized_msg, "%d", 0);
  strncat(serialized_msg, separator, 2);
  strncat(serialized_msg, msg->source_ip, strlen(msg->source_ip));
  strncat(serialized_msg, separator, 2);
  sprintf(temp, "%d", msg->source_port);
  strncat(serialized_msg, temp, strlen(temp));
  strncat(serialized_msg, separator, 2);
  strncat(serialized_msg, msg->destination_ip, strlen(msg->destination_ip));
  strncat(serialized_msg, separator, 2);
  sprintf(temp, "%d", msg->destination_port);
  strncat(serialized_msg, temp, strlen(temp));
  strncat(serialized_msg, separator, 2);
  strncat(serialized_msg, msg->payload, strlen(msg->payload));
  strcpy(m, serialized_msg);
}

// Terminal thread, display menu, get user input and redirect to chosen option
void *terminal(void *data) {
  Router *r = data; int option = -1;
  
  // Wait and check for user input
  while (1) {
    // display options
    printf("\n --------- Main Menu --------");
    printf("\n * 1 - Send Message");
    printf("\n * 2 - See Sent History");
    printf("\n * 3 - See Received History");
    printf("\n * 4 - Neighbours Info");
    printf("\n * 8 - Router Info");
    printf("\n * 9 - Clear Terminal");
    printf("\n * 0 - Exit");
    printf("\n ----------------------------");
    printf("\n option: ");
    scanf("%d", &option);

    if (option == 0) {
      clear_terminal();
      printf("\n Program finished...\n\n");
      exit(0);
    }
    else if (option == 1) {
      // Create 'sender' Thread
      pthread_t th_sender;
      pthread_create(&th_sender, NULL, (void *)sender, r);
      pthread_join(th_sender, NULL);
    }
    else if (option == 2) printf("WIP\n");
    else if (option == 3) display_received_msgs();
    else if (option == 4) display_neighbours_info(r);
    else if (option == 8) display_router_info(r);
    else if (option == 9) clear_terminal();
  }
}

// (Is a little foggy right now, on which is the pourpose)
void *packet_handler(void *data) {
  Router *r = data;
  
}

// Sender thread, is created only when called from Terminal thread
// allowing to get the user msg and send to a chosen destination
void *sender(void *data) {
  Router *r = data;

  int neighbour_option = -1, port = -1, temp = 0, i = 0, character;
  char msg_dest[20] = "", buffer[r->buffer_length], msg_serialized[100];
  memset(buffer, 0, strlen(buffer));  
  
  // (NOTE) In the future this will get all avaiable routers from the distance vector
  clear_terminal();
  printf("\n Select Neighbour:\n");
  // display a table with all avaiable router to connect with
  display_neighbours_info(r);
  // get user input
  printf(" Option (index): "); scanf("%d", &neighbour_option);

  Message *msg_object = malloc(sizeof(Message));
  strcpy(msg_object->source_ip, r->ip); msg_object->source_port = r->port;
  msg_object->type = 0;
  
  // Allow option to send msg to itself (debug/testing purposes)
  if (neighbour_option == -1) {
    // get port and set msg destination to yourself
    strcpy(msg_object->destination_ip, r->ip);
    msg_object->destination_port = r->port;
  }
  else {
    // set port to chosen neighbour and also set msg destination
    strcpy(msg_object->destination_ip, r->neighbours[neighbour_option]->ip);
    msg_object->destination_port = r->neighbours[neighbour_option]->port;
  }

  // Create UDP socket and configure settings
  int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(msg_object->destination_port);
  serverAddr.sin_addr.s_addr = inet_addr(msg_object->destination_ip);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
  socklen_t addr_size = sizeof(serverAddr);
  
  while(temp < 2) {
    // reset buffer
    memset(buffer, 0, strlen(buffer));  
    if (temp > 0) {
      clear_terminal();
      printf("\n ------------ Send Msg ------------");
      printf("\n Type msg to send to: ");
    }
  
    // get input into buffer
    while((character = getchar()) != '\n') { buffer[i++] = character; }
  
    // Send message to server
    if (temp > 0) {
      // Construct message object and serialize it
      strcpy(msg_object->payload, buffer);
  
      // basically get all attributes from Message and concatenate into a single string
      serialize_message(msg_serialized, msg_object);
      sendto(clientSocket, msg_serialized, r->buffer_length, 0, (struct sockaddr *)&serverAddr, addr_size);
      printf("\n Sending msg...\n");
    } temp ++;
  }
}

// Receiver thread, is always linstening to upcoming data on specified port
void *receiver(void *data) {
  Router *r = data;
  char buffer[r->buffer_length];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;

  // Create and configure UDP socket
  int udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(r->port);
  serverAddr.sin_addr.s_addr = inet_addr(r->ip);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  socklen_t addr_size = sizeof serverStorage;

  // Bind socket with address struct
  if (bind(udp_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    printf("Port Already Allocated\n"); exit(1);
  };

  while(1) {
    // Try to receive any upcoming UDP datagram. Address and port of
    // requesting client will be stored on serverStorage variable
    memset(buffer, 0, strlen(buffer));  
    recvfrom(udp_socket, buffer, r->buffer_length, 0, (struct sockaddr *)&serverStorage, &addr_size);
    queue_insert(buffer);
  }
}
