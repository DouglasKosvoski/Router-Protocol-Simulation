typedef struct Message {
  // whether msg is for sending a user-text or vector-control
  // 0 = msg; 1 = control
  short type;
  // ip address representing which router msg was originated from
  char source[20];
  // ip address representing msg router destination
  char destination[20];
  // message content
  char payload[100];
} Message;
