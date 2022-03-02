/*
* message.h
*/

typedef struct Message {
  // whether msg is for sending a user-text or vector-control
  // 0 = msg; 1 = control
  short type;
  char timestamp[20];
  // ip address representing which router msg was originated from
  char source_ip[20];
  int source_port;
  // ip address representing msg router destination
  char destination_ip[20];
  int destination_port;
  // message content
  char payload[256];
} Message;
