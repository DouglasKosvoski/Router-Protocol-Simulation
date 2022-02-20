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
#include <semaphore.h>

/* Custom structures and functions */
#include "load_config.h"
#include "neighbour.h"
#include "router.h"
#include "message.h"
#include "queue.h"
#include "routing_table.h"

/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

pthread_mutex_t in_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t out_mutex = PTHREAD_MUTEX_INITIALIZER;
Routing_table *rt; Router *r1;
Queue *q_in; Queue *q_out;

void send_initial_distance_vector();
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

/* Main */
int main(int argc, char const *argv[]) {
  // check if id was passed as argument
  check_arguments(argc);
  
  // allocate router
  r1 = malloc(sizeof(Router));
  rt = malloc(sizeof(Routing_table)); init_routing_table(rt);
  q_in = malloc(sizeof(Queue)); q_in->tail = -1;
  q_out = malloc(sizeof(Queue)); q_out->tail = -1;
  
  // configure router and tell neighbours its info 
  set_router(argv[1]); send_initial_distance_vector();
  // print some info onto the terminal, id, ip, port, pid
  display_router_info(r1);
  
  // Create threads
  pthread_t th_receiver, th_terminal, th_handler, th_sender;
  pthread_create(&th_terminal, NULL, (void *)terminal, NULL);
  pthread_create(&th_receiver, NULL, (void *)receiver, NULL);
  pthread_create(&th_handler, NULL, (void *)packet_handler, NULL);
  pthread_create(&th_sender, NULL, (void *)sender, NULL);
  
  // Join threads
  pthread_join(th_receiver, NULL);
  pthread_join(th_terminal, NULL);
  pthread_join(th_handler, NULL);
  pthread_join(th_sender, NULL);
  printf("Thread ID: %ld returned\n", th_receiver);
  printf("Thread ID: %ld returned\n", th_terminal);
  printf("Thread ID: %ld returned\n", th_handler);
  printf("Thread ID: %ld returned\n", th_sender);
  printf("\n*** Fim do programa ***\n");
  exit(0);
};

void send_initial_distance_vector() {
  char table_values[256], distance_vector[r1->buffer_length];
  Message *msg = malloc(sizeof(Message));
  
  // type 1 = distance vector
  msg->type = 1; strcpy(msg->source_ip, r1->ip); msg->source_port = r1->port;
  
  strcpy(msg->destination_ip, r1->ip); msg->destination_port = r1->port;
  
  // loop over neighbours here, setting destination accordinly
  // for (size_t i = 0; i < sizeof(r1->neighbours) / sizeof(r1->neighbours[0]); i++) {
  //   if (r1->neighbours[i]->id > 6) continue;
  //   strcpy(msg->destination_ip, r1->neighbours[i]->ip);
  //   msg->destination_port = r1->neighbours[i]->port;
  // }
  
  // set payload to distance vector content
  sply(rt, table_values);
  strcpy(msg->payload, " ");
  strcpy(msg->payload, table_values);
  
  serialize_message(distance_vector, msg);
  pthread_mutex_lock(&out_mutex);
  queue_insert(q_out, distance_vector);
  pthread_mutex_unlock(&out_mutex);
}

// Parse router config from cfg
void set_router(const char *i) {
  // id passed via argument on execution, convert to int
  r1->id = atoi(i);
  // get own process id
  r1->pid = getpid();
  // get parent process id
  r1->parent_pid = getppid();
  // set max buffer length for transfer
  r1->buffer_length = 1024;

  // set port and ip
  if (parse_router_config(roteador_cfg, r1->id, &r1->port, r1->ip) == -1) {
    printf("Router not Found!\n");
    exit(-1);
  }

  // Allocate and attach neighbours to router
  char s[20]; parse_enlaces_config(enlaces_cfg, r1->id, s);
  // alloc Neighbours in memory
  Neighbour *n1 = malloc(sizeof(Neighbour));
  Neighbour *n2 = malloc(sizeof(Neighbour));
  Neighbour *n3 = malloc(sizeof(Neighbour));
  r1->neighbours[0] = n1; r1->neighbours[1] = n2; r1->neighbours[2] = n3;
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);
  routing_table_set(rt, r1->id, 0, r1->id);


  // Set ip and ports
  for (size_t i = 0; i < sizeof(r1->neighbours) / sizeof(r1->neighbours[0]); i++) {
    if (parse_router_config(roteador_cfg, r1->neighbours[i]->id, &r1->neighbours[i]->port, r1->neighbours[i]->ip) == -1) {
      continue;
    } else {
      // from_id, cost, next_hop_id;
      routing_table_set(rt, r1->neighbours[i]->id, r1->neighbours[i]->cost, r1->neighbours[i]->id);
    }
  }
}

// Display information about the router
void display_router_info() {
  clear_terminal();
  printf("\n ----------- INFO -----------\n");
  printf(" Router  id   : %13d \n", r1->id);
  printf(" Process id   : %13d \n", r1->pid);
  printf(" Parent pid   : %13d \n", r1->parent_pid);
  printf(" Buffer len   : %13d \n", r1->buffer_length);
  printf(" Port         : %13d \n", r1->port);
  printf(" IP addr      :     %s\n",  r1->ip);
  printf(" ----------------------------\n\n");
}

// Loop over all neighbours and display useful info about 'em (and itself)
void display_neighbours_info() {
  clear_terminal(); printf("\n Neighbours Information");
  printf("\n  ------------------------------------------\n");
  printf(" | index | id \t| cost \t| port \t|     ip    |");
  printf("\n  -------------------------------------------\n");
  printf(" (self)");
  printf("\n  ------------------------------------------\n");
  printf(" |  %d   | %d \t| %d \t| %d | %s | \n", -1, r1->id, 0, r1->port, r1->ip);
  printf("  ------------------------------------------\n");
  printf(" (neighbours)\n");
  printf("  ------------------------------------------\n");

  // Loop over neighbours and print their info
  for (size_t i = 0; i < sizeof(r1->neighbours) / sizeof(r1->neighbours[0]); i++) {
    if (r1->neighbours[i]->cost > 15) { continue; };
    printf(" |   %d \t | %d \t| %d \t| %d | %s | \n", i, r1->neighbours[i]->id, r1->neighbours[i]->cost, r1->neighbours[i]->port, r1->neighbours[i]->ip);
  } printf("  ------------------------------------------\n");
}

// Get all attributes from Message and concatenate into a single string
void serialize_message(char *m, Message *msg) {
  char serialized_msg[1024] = "";
  char separator[2] = "^";
  char temp[20];

  // set type
  sprintf(serialized_msg, "%d", msg->type);
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

void deserialize_msg(Message *msg, char *serialized_msg) {
  char temp[1024];
  strcpy(temp, serialized_msg);
  char *delim = "^";
  char *token = strtok(temp, "^");
  
  int counter = 0;
  while(token != NULL) {
    if (counter == 0) {
      msg->type = atoi(token);
    } else if (counter == 1) {
      strcpy(msg->source_ip, token);
    } else if (counter == 2) {
      msg->source_port = atoi(token);
    } else if (counter == 3) {
      strcpy(msg->destination_ip, token);
    } else if (counter == 4) {
      msg->destination_port = atoi(token);
    } else if (counter == 5) {
      strcpy(msg->payload, token);
    }
    counter++;
    token = strtok(NULL, delim);
  }
}

void get_user_message() {
  int neighbour_option = -1;
  char msg_serialized[100];
  
  // (NOTE) In the future this will get all avaiable routers from the distance vector
  clear_terminal(); printf("\n Select Neighbour:\n");
  // display a table with all avaiable router to connect with
  display_neighbours_info(r1);
  // get user input
  printf(" Option (index): "); scanf("%d", &neighbour_option);
  printf("Neighbout Chorse: %d\n", neighbour_option);
  Message *msg_object = malloc(sizeof(Message));
  strcpy(msg_object->source_ip, r1->ip);
  msg_object->source_port = r1->port; msg_object->type = 0;
  
  // Allow option to send msg to itself (debug/testing purposes)
  if (neighbour_option == -1) {
    // get port and set msg destination to yourself
    strcpy(msg_object->destination_ip, r1->ip);
    msg_object->destination_port = r1->port;
  }
  else {
    // set port to chosen neighbour and also set msg destination
    strcpy(msg_object->destination_ip, r1->neighbours[neighbour_option]->ip);
    msg_object->destination_port = r1->neighbours[neighbour_option]->port;
  }
  
  clear_terminal();
  printf("\n ------------ Send Msg ------------");
  printf("\n Type msg to send to: ");
  scanf("%s", &msg_object->payload);
  // basically get all attributes from Message and concatenate into a single string
  serialize_message(msg_serialized, msg_object);
  
  // add message to outgoing queue
  pthread_mutex_lock(&out_mutex);
  queue_insert(q_out, msg_serialized);
  pthread_mutex_unlock(&out_mutex);
}

// Terminal thread, display menu, get user input and redirect to chosen option
void *terminal(void *) {
  int option = -1;
  // Wait and check for user input
  while (1) {
    // display options
    printf("\n --------- Main Menu --------");
    printf("\n * 1 - Send Message");
    printf("\n * 2 - See Out Queue");
    printf("\n * 3 - See In Queue");
    printf("\n * 4 - Neighbours Info");
    printf("\n * 7 - Routing Table");
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
    else if (option == 1) get_user_message();
    else if (option == 2) { clear_terminal(); display_queue_content(q_out);}
    else if (option == 3) { clear_terminal(); display_queue_content(q_in);}
    else if (option == 4) display_neighbours_info(r1);
    else if (option == 7) { clear_terminal(); display_routing_table(rt); }
    else if (option == 8) display_router_info(r1);
    else if (option == 9) clear_terminal();
  }
}

void *packet_handler(void *) {
  Message *msg = malloc(sizeof(Message));
  char deserialized_msg[r1->buffer_length];

  while (1) {
    // listen to incoming queue
    pthread_mutex_lock(&in_mutex);
    
    if (queue_size(q_in) > 0) {
      strcpy(deserialized_msg, queue_start(q_in));
      deserialize_msg(msg, deserialized_msg);
      
      // if message final destination is to this router, else forward
      if (strncmp(r1->ip, msg->destination_ip, strlen(msg->destination_ip)) == 0 && r1->port == msg->destination_port) {
        printf("Mensagem pra mim\n");
        
        // user message
        if (msg->type == 0) {
          printf("Voce tem uma nova mensagem\n");
        }
        // distance vector
        else {
          printf("Recebido Vetor distancia\n");
          printf("SRC: '%s:%d'\n", msg->source_ip, msg->source_port);
          printf("DST: '%s:%d'\n", msg->destination_ip, msg->destination_port);
          printf("PLD: '%s'   \n", msg->payload);
        }
      }
      // forward message to correct destination
      else {
        printf("Encaminhar MSG\n");
      }
      
      queue_remove(q_in);
      // exit(0);
    }
    
    pthread_mutex_unlock(&in_mutex);
  }
}

// Sender thread, is created only when called from Terminal thread
// allowing to get the user msg and send to a chosen destination
void *sender(void *) {
  Message *msg = malloc(sizeof(Message));
  char serialized_msg[r1->buffer_length];
  
  while (1) {
    if (queue_size(q_out) > 0) {
      pthread_mutex_lock(&out_mutex);
      strcpy(serialized_msg, queue_start(q_out));
      deserialize_msg(msg, serialized_msg);
      
      // Create UDP socket and configure settings
      int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
      struct sockaddr_in serverAddr; serverAddr.sin_family = AF_INET;
      serverAddr.sin_port = htons(msg->destination_port);
      serverAddr.sin_addr.s_addr = inet_addr(msg->destination_ip);
      memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
      socklen_t addr_size = sizeof(serverAddr);
      
      printf("\nSending MSG...\n");
      sendto(clientSocket, serialized_msg, r1->buffer_length, 0, (struct sockaddr *)&serverAddr, addr_size);
      queue_remove(q_out);
      pthread_mutex_unlock(&out_mutex);
    }
  }
}

// Receiver thread, is always linstening to upcoming data on specified port
void *receiver(void *) {
  char buffer[r1->buffer_length];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;

  // Create and configure UDP socket
  int udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(r1->port);
  serverAddr.sin_addr.s_addr = inet_addr(r1->ip);
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
    recvfrom(udp_socket, buffer, r1->buffer_length, 0, (struct sockaddr *)&serverStorage, &addr_size);
    
    // add message to incoming queue
    pthread_mutex_lock(&in_mutex);
    printf("\nMSG Received...\n");
    queue_insert(q_in, buffer);
    pthread_mutex_unlock(&in_mutex);
  }
}
