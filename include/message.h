/*
* message.h
*/

static int msg_id = 0; 

typedef struct Message {
  // whether msg is for sending a user-text or vector-control
  // 0 = msg; 1 = control
  short type; char timestamp[20];
  
  // ip address representing which router msg was originated from
  char source_ip[20]; int source_port;
  
  // ip address representing msg next hop destination
  char next_hop_destination_ip[20]; int next_hop_destination_port;
  
  // id of the final router destination
  int final_destination_id;
  // message content
  char payload[256];
  int id;
} Message;
