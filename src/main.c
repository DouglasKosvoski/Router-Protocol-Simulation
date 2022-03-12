/*
* main.c
*/

/*
* Author `Douglas Kosvoski`
* Class `Redes de Computadores 2021.2`
*
* Router protocol simulation via UDP sockets
* and Distributed Bellman-Ford Algorithm
*
* 2022/1
*/

#include "main.h"

int main(int argc, char const *argv[]) {
  clear_terminal();
  
  // check if id was passed as argument on execution
  if (check_arguments(argc) == -1) {
    printf("\nRouter Id expected !!!\n");
    printf("Ex: `./router 1` \n\n");
    return -1;
  }
  
  // allocate Router, Routing_table and Queues
  r1 = malloc(sizeof(Router));
  rt = malloc(sizeof(Routing_table)); init_routing_table(rt);
  q_in = malloc(sizeof(Queue)); queue_init(q_in);
  q_out = malloc(sizeof(Queue)); queue_init(q_out);
  user_messages_received = malloc(sizeof(Queue));
  queue_init(user_messages_received);
  
  // configure router and tell neighbours its info 
  if (set_router(argv[1]) == -1) {
    printf("Router not Found!\n");
    return -1;
  }
  
  // Create threads
  pthread_t th_receiver, th_terminal, th_handler, th_sender, th_send_rt_periodically;
  pthread_create(&th_terminal, NULL, (void *)terminal, NULL);
  pthread_create(&th_receiver, NULL, (void *)receiver, NULL);
  pthread_create(&th_handler, NULL, (void *)packet_handler, NULL);
  pthread_create(&th_sender, NULL, (void *)sender, NULL);
  pthread_create(&th_send_rt_periodically, NULL, (void *)send_rt_periodically, NULL);
  
  // Join threads
  pthread_join(th_receiver, NULL); pthread_join(th_terminal, NULL);
  pthread_join(th_handler, NULL); pthread_join(th_sender, NULL);
  pthread_join(th_send_rt_periodically, NULL);
  return 0;
};

void get_timestamp(char *tt) {
  char temp[20];
  time_t t = time(NULL); struct tm tm = *localtime(&t);
  snprintf(temp, sizeof(temp), "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  strcpy(tt, temp);
}

// Send distance vector to all attached neighbours
void send_distance_vector() {
  char table_values[256], distance_vector[r1->buffer_length], temp[20];
  Message *msg = malloc(sizeof(Message));
  
  // type 1 = distance vector
  msg->type = 1; strcpy(msg->source_ip, r1->ip); msg->source_port = r1->port;
  get_timestamp(msg->timestamp);
  
  // loop over neighbours here, setting destination accordinly
  for (size_t i = 0; i < sizeof(r1->neighbours) / sizeof(r1->neighbours[0]); i++) {
    if (r1->neighbours[i]->id > 6) continue;
    strcpy(msg->next_hop_destination_ip, r1->neighbours[i]->ip);
    msg->next_hop_destination_port = r1->neighbours[i]->port;
    msg->final_destination_id = r1->neighbours[i]->id;
    
    // set payload to distance vector content
    sply(rt, table_values);
    strcpy(msg->payload, " ");
    strcpy(msg->payload, table_values);

    serialize_message(distance_vector, msg);
    pthread_mutex_lock(&out_mutex);
    queue_insert(q_out, distance_vector);
    pthread_mutex_unlock(&out_mutex);
  }
}

// Parse router config from cfg
int set_router(const char *i) {
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
    return -1;
  }

  // Allocate and attach neighbours to router
  char s[20]; parse_enlaces_config(enlaces_cfg, r1->id, s);

  // alloc Neighbours in memory
  Neighbour *n1, *n2, *n3;
  n1 = malloc(sizeof(Neighbour)); r1->neighbours[0] = n1;
  n2 = malloc(sizeof(Neighbour)); r1->neighbours[1] = n2;
  n3 = malloc(sizeof(Neighbour)); r1->neighbours[2] = n3;
  
  sscanf(s, "%d %d %d %d %d %d", &n1->id, &n1->cost, &n2->id, &n2->cost, &n3->id, &n3->cost);
  routing_table_set(rt, r1->id, 0, r1->id);

  // Set ip and ports
  for (size_t i = 0; i < sizeof(r1->neighbours) / sizeof(r1->neighbours[0]); i++) {
    if (parse_router_config(roteador_cfg, r1->neighbours[i]->id, &r1->neighbours[i]->port, r1->neighbours[i]->ip) == -1) {
      continue;
    }
    else {
      // from_id, cost, next_hop_id;
      routing_table_set(rt, r1->neighbours[i]->id, r1->neighbours[i]->cost, r1->neighbours[i]->id);
    }
  }
  return 0;
}

// Display information about the router
void display_router_info() {
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
void display_reachable_routers() {
  printf("\n  Index | Cost | Next hop\n");
  printf("  ------------------------\n");

  for (size_t i = 1; i < SIZE+1; i++) {
    if (rt->routes[i][0] != INF) {
      if (i == r1->id) {
        printf("    %ld* \t| %3d  | %5d \n", i, rt->routes[i][0], rt->routes[i][1]);
      }
      else {
        printf("    %ld \t| %3d  | %5d \n", i, rt->routes[i][0], rt->routes[i][1]);
      }
    }
  }
  printf("  ------------------------\n");
}

// Get all attributes from Message and concatenate into a single string
void serialize_message(char *m, Message *msg) {
  char serialized_msg[1024] = "", separator[2] = "^", temp[20];

  // message type
  sprintf(serialized_msg, "%d", msg->type);
  strncat(serialized_msg, separator, 2);
  // message timestamp
  strncat(serialized_msg, msg->timestamp, strlen(msg->timestamp));
  strncat(serialized_msg, separator, 2);
  // message origin address
  strncat(serialized_msg, msg->source_ip, strlen(msg->source_ip));
  strncat(serialized_msg, separator, 2);
  sprintf(temp, "%d", msg->source_port);
  strncat(serialized_msg, temp, strlen(temp));
  strncat(serialized_msg, separator, 2);
  
  // message next hop source ip and port
  strncat(serialized_msg, msg->next_hop_destination_ip, strlen(msg->next_hop_destination_ip));
  strncat(serialized_msg, separator, 2);
  sprintf(temp, "%d", msg->next_hop_destination_port);
  strncat(serialized_msg, temp, strlen(temp));
  
  strncat(serialized_msg, separator, 2);
  sprintf(temp, "%d", msg->final_destination_id);
  strncat(serialized_msg, temp, strlen(temp));
  
  strncat(serialized_msg, separator, 2);
  strncat(serialized_msg, msg->payload, strlen(msg->payload));
  
  strcpy(m, serialized_msg);
}

// Parse string into Message object
void deserialize_msg(Message *msg, char *serialized_msg) {
  char temp[1024], *delim = "^";
  int counter = 0;
  
  strcpy(temp, serialized_msg);
  char *token = strtok(temp, "^");
  
  // go through the whole string
  while(token != NULL) {
    if (counter == 0) {
      msg->type = atoi(token);
    } else if (counter == 1) {
      strcpy(msg->timestamp, token);
    } else if (counter == 2) {
      strcpy(msg->source_ip, token);
    } else if (counter == 3) {
      msg->source_port = atoi(token);
    } else if (counter == 4) {
      strcpy(msg->next_hop_destination_ip, token);
    } else if (counter == 5) {
      msg->next_hop_destination_port = atoi(token);
    } else if (counter == 6) {
      msg->final_destination_id = atoi(token);
    } else if (counter == 7) {
      strcpy(msg->payload, token);
    }
    counter++;
    token = strtok(NULL, delim);
  }
}

// Get user msg input from terminal
void get_user_message() {
  int neighbour_option = -1;
  char msg_serialized[100];
  
  // (NOTE) In the future this will get all avaiable routers from the distance vector
  printf("\n Select Neighbour:\n");
  // display a table with all avaiable router to connect with
  display_reachable_routers(r1);
  // get user input
  printf(" Option (index): "); scanf("%d", &neighbour_option);
  
  int neighbour_valid = 0;
  for (size_t i = 1; i < SIZE+1; i++) {
    if (rt->routes[i][0] != INF) {
      if (i == neighbour_option) {
        neighbour_valid = 1;
      }
    }
  }
  if (!neighbour_valid) {
    printf("\n Invalid Input\n");
    return;
  }
  
  Message *msg = malloc(sizeof(Message));
  get_timestamp(msg->timestamp);
  strcpy(msg->source_ip, r1->ip);
  msg->source_port = r1->port; msg->type = 0;
  msg->final_destination_id = neighbour_option;
  
  if (msg->final_destination_id == r1->id) {
    // get port and set msg destination to yourself
    strcpy(msg->next_hop_destination_ip, r1->ip);
    msg->next_hop_destination_port = r1->port;
  }
  else {
    for (size_t i = 0; i < sizeof(r1->neighbours)/sizeof(r1->neighbours[0]); i++) {
      if (r1->neighbours[i]->id == rt->routes[msg->final_destination_id][1]) {
        // set port to chosen neighbour and also set msg destination
        strcpy(msg->next_hop_destination_ip, r1->neighbours[i]->ip);
        msg->next_hop_destination_port = r1->neighbours[i]->port;
      }
    }
  }
  
  clear_terminal();
  printf("\n ------------ Send Msg ------------\n Type msg to send to: ");
  scanf("%s", &msg->payload);
  
  // basically get all attributes from Message and concatenate into a single string
  serialize_message(msg_serialized, msg);
  
  // add message to outgoing queue
  pthread_mutex_lock(&out_mutex);
  queue_insert(q_out, msg_serialized);
  pthread_mutex_unlock(&out_mutex);
}

void display_received_messages() {
  if (queue_size(user_messages_received) == 0) {
    printf("\n ----------------------------");
    printf("\n -> Queue is Empty \n");
    printf(" ----------------------------\n");
    return;
  }
  Message *m = malloc(sizeof(Message));
  char deserialized_msg[r1->buffer_length];
  
  printf(" \tFrom \t  |     Content  | \tTimestamp\n");
  for (size_t i = 0; i < queue_size(user_messages_received); i++) {
    strcpy(deserialized_msg, user_messages_received->queue[i]);
    deserialize_msg(m, deserialized_msg);
    printf(" %10s:%d | %12s | %s\n", m->source_ip, m->source_port, m->payload, m->timestamp);
  }
}

// Terminal thread, display menu, get user input and redirect to chosen option
void *terminal(void *) {
  int option = -1;
  // Wait and check for user input
  while (1) {
    // display options
    printf("\n --------- Main Menu --------");
    printf("\n * 1 - Send Message");
    printf("\n * 2 - Display Reachable Routers");
    printf("\n * 3 - Send Distance Vector");
    printf("\n * 4 - Display Routing Table");
    printf("\n * 5 - Router Info");
    printf("\n * 6 - See Received Messages");
    printf("\n * 9 - Clear Terminal");
    printf("\n * 0 - Exit");
    printf("\n ----------------------------");
    printf("\n option: "); scanf("%d", &option);

    clear_terminal();
    if (option == 0) {
      printf("\n Program finished...\n\n");
      exit(0);
    }
    else if (option == 1) get_user_message();
    else if (option == 2) display_reachable_routers(r1);
    else if (option == 3) send_distance_vector();
    else if (option == 4) display_routing_table(rt);
    else if (option == 5) display_router_info(r1);
    else if (option == 6) display_received_messages();
    else if (option == 9) clear_terminal();
  }
}

// Manage all incoming data
void *packet_handler(void *) {
  Message *msg = malloc(sizeof(Message));
  char deserialized_msg[r1->buffer_length];

  while (1) {
    // listen to incoming queue
    pthread_mutex_lock(&in_mutex);
    
    // if there is anything on incoming queue
    if (queue_size(q_in) > 0) {
      strcpy(deserialized_msg, queue_start(q_in));
      deserialize_msg(msg, deserialized_msg);
      
      // if message final destination is to this router, else forward
      if (r1->id == msg->final_destination_id) {
        // if is user message
        if (msg->type == 0) {
          printf("\n You have a new message\n");
          queue_insert(user_messages_received, queue_start(q_in));
        }
        // if is distance vector
        else {
          printf("\n Distance vector arrived from {%s:%d}\n", msg->source_ip, msg->source_port);
          if (bellman_ford_distributed(rt, msg->payload, msg->source_port % 10) == 1) {
            char temp[20]; get_timestamp(temp);
            printf("\n Routing Updated {%s}", temp); display_routing_table(rt);
            send_distance_vector();
          };
        }
      }
      // forward message to correct destination, since I am not final destination
      else {
        char deserialized_msg[r1->buffer_length], serialized_msg[r1->buffer_length];
        
        printf("\n Forwarding from {%s:%d} to {%s:%d}\n", msg->source_ip, msg->source_port, msg->next_hop_destination_ip, msg->next_hop_destination_port, msg->final_destination_id);
        strcpy(deserialized_msg, queue_start(q_in));
        deserialize_msg(msg, deserialized_msg);

        for (size_t i = 0; i < sizeof(r1->neighbours)/sizeof(r1->neighbours[0]); i++) {
          if (r1->neighbours[i]->id == rt->routes[msg->final_destination_id][1]) {
            strcpy(msg->next_hop_destination_ip, r1->neighbours[i]->ip);
            msg->next_hop_destination_port = r1->neighbours[i]->port;
          }
        }
        
        serialize_message(serialized_msg, msg);

        pthread_mutex_lock(&out_mutex);
        queue_insert(q_out, serialized_msg);
        pthread_mutex_unlock(&out_mutex);
      }
      queue_remove(q_in);
    }
    pthread_mutex_unlock(&in_mutex);
  }
}

// Sender Thread keep on listening to outgoing queue and send msg when there is anything
void *sender(void *) {
  Message *msg = malloc(sizeof(Message));
  char serialized_msg[r1->buffer_length];
  
  while (1) {
    pthread_mutex_lock(&out_mutex);
    if (queue_size(q_out) > 0) {
      strcpy(serialized_msg, queue_start(q_out));
      deserialize_msg(msg, serialized_msg);
      
      // Create UDP socket and configure settings
      int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
      struct sockaddr_in serverAddr; serverAddr.sin_family = AF_INET;
      serverAddr.sin_port = htons(msg->next_hop_destination_port);
      serverAddr.sin_addr.s_addr = inet_addr(msg->next_hop_destination_ip);
      memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
      socklen_t addr_size = sizeof(serverAddr);

      printf("\n Sending msg to {%s:%d}\n", msg->next_hop_destination_ip, msg->next_hop_destination_port);
      sendto(clientSocket, serialized_msg, r1->buffer_length, 0, (struct sockaddr *)&serverAddr, addr_size);
      queue_remove(q_out);
    }
    pthread_mutex_unlock(&out_mutex);
  }
}

// Receiver thread, is always linstening to incoming data on specified port
void *receiver(void *) {
  char buffer[r1->buffer_length];
  struct sockaddr_in serverAddr;
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

  // Try to receive any upcoming UDP datagram. Address and port of
  // requesting client will be stored on serverStorage variable
  while(1) {
    // clear buffer
    memset(buffer, 0, strlen(buffer));
    // receive data
    recvfrom(udp_socket, buffer, r1->buffer_length, 0, (struct sockaddr *)&serverStorage, &addr_size);
    
    // add message to incoming queue
    pthread_mutex_lock(&in_mutex);
    printf("`%s`\n", buffer); exit(0);
    queue_insert(q_in, buffer);
    pthread_mutex_unlock(&in_mutex);
  }
}

void *send_rt_periodically(void *) {
  int interval = 10;
  while (1) {
    sleep(interval);
    send_distance_vector();
  }
}