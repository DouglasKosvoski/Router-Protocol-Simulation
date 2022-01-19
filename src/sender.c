void *sender(void *data) {
  Router *r = data;
  struct sockaddr_in si_me, si_other;
  int udp_socket, recv_len, slen = sizeof(si_other);
  char buffer[r->buffer_length];

  // create UDP socket
  udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // if couldn't create
  if (udp_socket == -1) exit(1);

  // zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));
  // The port on which to listen for incoming data
  si_me.sin_port = htons(r->port);
  si_me.sin_family = AF_INET;
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind socket to port
  if (bind(udp_socket, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) exit(1);

  // keep listening for data
  while(1) {
    printf("Listening port %d... ", r->port);
    fflush(stdout);
    // receive a reply and print it
    // clear the buffer by filling null, it might have previously received data
    memset(buffer,'\0', r->buffer_length);

    // try to receive some data, this is a blocking call
    recv_len = recvfrom(udp_socket, buffer, r->buffer_length, 0, (struct sockaddr *) &si_other, &slen);
    if (recv_len == -1) exit(1);

    //print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data: %s\n", buffer);

    // now reply the client with the same data
    if (sendto(udp_socket, buffer, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) exit(1);
  }
  pclose(udp_socket);
}
