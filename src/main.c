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

/* Custom structures and functions*/
#include "load_config.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

/* Functions */
void check_arguments(int args);
void set_neighbours(Router *r);
void set_router(Router *r, const char *i);
void display_router_info(Router *r);
void *sender(void *data);
void *receiver(void *data);

/* Main */
int main(int argc, char const *argv[]) {
  // check if id was passed as argument
  check_arguments(argc);

  // allocate router and threads
  Router *r1 = malloc(sizeof(Router));

  // set buffer
  set_router(r1, argv[1]);
  // print some info onto the terminal, id, ip, port, pid
  display_router_info(r1);

  // Create threads
  pthread_t th_receiver, th_sender; //, th_packet_handler, th_terminal
  pthread_create(&th_receiver, NULL, (void *)receiver, r1);
  pthread_create(&th_sender, NULL, (void *)sender, r1);

  // Join threads
  pthread_join(th_receiver, NULL);
  pthread_join(th_sender, NULL);
  printf("Thread ID: %ld returned\n", th_receiver);
  printf("Thread ID: %ld returned\n", th_sender);

  printf("\n*** Fim do programa ***\n");
  exit(0);
};

// Check if router id was passed on execution
void check_arguments(int args) {
  if (args < 2) {
    printf("\nRouter Id expected !!!\n");
    printf("Ex: `./router 4` \n\n");
    exit(1);
  }
}

// Allocate and attach neighbours to router
void set_neighbours(Router *r) {
  // if router does not have 3 neighbours
  // unused Neighbour will be set to id=0 and cost=0
  char s[20];
  parse_enlaces_config(enlaces_cfg, r->id, s);

  // alloc Neighbours in memory
  Neighbour *n1 = malloc(sizeof(Neighbour));
  Neighbour *n2 = malloc(sizeof(Neighbour));
  Neighbour *n3 = malloc(sizeof(Neighbour));
  // set and attach neighbours to router
  r->neighbours[0] = n1; r->neighbours[1] = n2; r->neighbours[2] = n3;
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);
}

// Parse router config from cfg
void set_router(Router *r, const char *i) {
  // id passed via argument on execution, convert to int
  r->id = atoi(i);
  // get own process id
  r->pid = getpid();
  // get parent process id
  r->parent_pid = getppid();
  // set max buffer lenght for transfer
  r->buffer_length = 512;
  // set port and ip
  parse_router_config(roteador_cfg, r->id, &r->port, r->ip);
  // allocate and attach neighbours to router
  set_neighbours(r);
}

// Display information about the router
void display_router_info(Router *r) {
  printf("\n----------- INFO -----------\n");
  printf("Router  id   : %13d \n", r->id);
  printf("Process id   : %13d \n", r->pid);
  printf("Parent pid   : %13d \n", r->parent_pid);
  printf("Buffer len   : %13d \n", r->buffer_length);
  printf("Port         : %13d \n", r->port);
  printf("IP addr      :     %s",  r->ip);
  printf("----------------------------\n\n");
}

void *sender(void *data) {
  Router *r = data;
  char buffer[r->buffer_length];
  int port = 25001;

  /*Create UDP socket*/
  int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = inet_addr(r->ip);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  /*Initialize size variable to be used later on*/
  socklen_t addr_size = sizeof serverAddr;

  while(1) {
    printf("client - Type a sentence to send to server: ");
    fgets(buffer, r->buffer_length, stdin);
    int nBytes = strlen(buffer) + 1;

    /*Send message to server*/
    sendto(clientSocket, buffer, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
    printf("client - Sending msg: %s", buffer);
    /*Receive message from server*/
    nBytes = recvfrom(clientSocket, buffer, r->buffer_length, 0, NULL, NULL);
    printf("client - Received from server: 127.0.0.1 (%d) %s", port, buffer);
  }
}

void *receiver(void *data) {
  Router *r = data;
  char buffer[r->buffer_length];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;

  /*Create UDP socket*/
  int udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(r->port);
  serverAddr.sin_addr.s_addr = inet_addr(r->ip);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*Bind socket with address struct*/
  bind(udp_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  /*Initialize size variable to be used later on*/
  socklen_t addr_size = sizeof serverStorage;

  while(1) {
    /* Try to receive any incoming UDP datagram. Address and port of
    requesting client will be stored on serverStorage variable */
    int nBytes = recvfrom(udp_socket, buffer, r->buffer_length, 0, (struct sockaddr *)&serverStorage, &addr_size);
    printf("server - Received msg: %s", buffer);

    /*Send message back to client, using serverStorage as the address*/
    printf("server - Sending msg back to client: %s", buffer);
    sendto(udp_socket, buffer, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
  }
}
