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
void *terminal(void *data);
void *packet_handler();

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
  pthread_t th_receiver, th_sender, th_packet_handler, th_terminal;
  pthread_create(&th_receiver, NULL, (void *)receiver, r1);
  // pthread_create(&th_sender, NULL, (void *)sender, r1);
  pthread_create(&th_packet_handler, NULL, (void *)packet_handler, NULL);
  pthread_create(&th_terminal, NULL, (void *)terminal, r1);

  // Join threads
  pthread_join(th_receiver, NULL);
  // pthread_join(th_sender, NULL);
  pthread_join(th_packet_handler, NULL);
  pthread_join(th_terminal, NULL);
  printf("Thread ID: %ld returned\n", th_receiver);
  // printf("Thread ID: %ld returned\n", th_sender);
  printf("Thread ID: %ld returned\n", th_packet_handler);
  printf("Thread ID: %ld returned\n", th_terminal);

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

void *terminal(void *data) {
  Router *r = data;

  int option = -1;
  system("clear");
  while (1) {
    printf("\n --------- Main Menu ---------");
    printf("\n * 1 - Send Message");
    printf("\n * 2 - See Sent History");
    printf("\n * 3 - See Received History");
    printf("\n * 0 - Exit");
    printf("\n -----------------------------");
    printf("\n option: ");
    scanf("%d", &option);

    if (option == 0) {
      system("clear");
      printf("Program finished...\n");
      exit(0);
    }
    else if (option == 1) {
      pthread_t th_sender;
      pthread_create(&th_sender, NULL, (void *)sender, r);
      pthread_join(th_sender, NULL);

    }
    else if (option == 2) printf("WIP\n");
    else if (option == 3) printf("WIP\n");
  }
}

void *packet_handler() {
  // printf("Salve2\n");
}

void *sender(void *data) {
  Router *r = data;
  char buffer[r->buffer_length];
  int port = r->port;
  int temp = 0;

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

  while(temp < 2) {
    // clear buffer
    memset(buffer, 0, strlen(buffer));
    int ch, i = 0;

    if (temp > 0) {
      system("clear");
      printf("\n ------------ Send Msg ------------");
      printf("\n Type msg to send to: ");
    }
    while((ch = getchar()) != '\n') {
      buffer[i++] = ch;
    } buffer[i++] = '\n';


    if (temp > 0) {
      int nBytes = strlen(buffer) + 1;
      /*Send message to server*/
      sendto(clientSocket, buffer, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
      printf("\n Sending msg...", buffer);
      /*Receive message from server*/
      nBytes = recvfrom(clientSocket, buffer, r->buffer_length, 0, NULL, NULL);
      // printf("\n client - Received from server: 127.0.0.1 (%d) %s", port, buffer);
    }
    temp ++;
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
  if (bind(udp_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    printf("Porta ja alocada\n");
    exit(1);
  };
  /*Initialize size variable to be used later on*/
  socklen_t addr_size = sizeof serverStorage;

  while(1) {
    /* Try to receive any incoming UDP datagram. Address and port of
    requesting client will be stored on serverStorage variable */
    int nBytes = recvfrom(udp_socket, buffer, r->buffer_length, 0, (struct sockaddr *)&serverStorage, &addr_size);
    printf("\n Received msg: %s", buffer);

    /*Send message back to client, using serverStorage as the address*/
    sendto(udp_socket, buffer, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
  }
}
