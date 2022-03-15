/*
 * message.h
 */

static int msg_id = 0;

typedef struct Message
{
  // whether msg is for sending a user-text or vector-control, 0 = msg; 1 = control
  short type;
  // datetime representing when the Message was created
  char timestamp[20];

  // ip address representing which router msg was originated from
  char source_ip[20];
  // port representing which router msg was originated from
  int source_port;

  // ip address representing msg next hop destination
  char next_hop_destination_ip[20];
  // port address representing msg next hop destination
  int next_hop_destination_port;

  // id of the final router destination
  int final_destination_id;
  // message content
  char payload[256];
  // id which represent a counter
  int id;

} Message;
