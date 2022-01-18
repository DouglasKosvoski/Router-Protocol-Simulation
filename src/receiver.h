#define SERVER "127.0.0.1"
#define PORT 8888

void *receiver(Router *r) {
  struct sockaddr_in si_other;
  int udp_socket, i, slen = sizeof(si_other);
  char buffer[r->buffer_length];
  char message[r->buffer_length];

  udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp_socket == -1) exit(1);

  // zero out the structure
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);

  if (inet_aton(SERVER, &si_other.sin_addr) == 0)  {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  while(1) {
    printf("Enter message: "); scanf("%s", message);
    // send the message
    if (sendto(udp_socket, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == -1) exit(1);

    // receive a reply and print it
    // clear the buffer by filling null, it might have previously received data
    memset(buffer,'\0', r->buffer_length);
    // try to receive some data, this is a blocking call
    printf("R:%s\n", message);
    if (recvfrom(udp_socket, buffer, r->buffer_length, 0, (struct sockaddr *) &si_other, &slen) == -1) exit(1);
    puts(buffer);
  }
  pclose(udp_socket);
}
