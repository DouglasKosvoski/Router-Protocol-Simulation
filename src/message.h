typedef struct Message {
  short msg_type;
  char source[20];
  char destination[20];
  char payload[100];
} Message;
