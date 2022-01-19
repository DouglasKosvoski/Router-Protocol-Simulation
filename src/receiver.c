#include <stdio.h>      // input/output
#include <stdlib.h>     // data convertions
#include <sys/types.h>  // data types
#include <unistd.h>     // constant and types
#include <arpa/inet.h>  // internet operations
#include <sys/socket.h> // sockets definitions
#include <pthread.h>    // thread manipulation
#include <string.h>     // string manipulation

#define SERVER "127.0.0.1"
#define PORT 8888
