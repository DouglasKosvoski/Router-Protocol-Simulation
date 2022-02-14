#include <stdio.h>      // input/output
#include <stdlib.h>     // data convertions
#include <sys/types.h>  // data types
#include <unistd.h>     // constant and types
#include <string.h>

// Check if string starts with a specific sufix
int startswith(const char *original, const char *sufix) {
   if(strncmp(original, sufix, strlen(sufix)) == 0) {
     return 1;
   };
   return 0;
}

// Responsible for spliting a buffer at a delimiter and converting char[] into an integer
void parse_line(char *buff, const char delim[1], char *ori, char *dest, char *cost) {
  char *token = strtok(buff, delim);
  int counter = 0;
  while(token != NULL) {
    if (counter == 0) {
      strcpy(ori, token);
    }
    else if (counter == 1) {
      strcpy(dest, token);
    }
    else if (counter == 2) {
      strcpy(cost, token);
    }
    counter++;
    token = strtok(NULL, delim);
  }
}

void parse_enlaces_config(char filename[], int rid, char *v) {
  FILE* filePointer;
  int buffer_size = 255;
  char buffer[buffer_size];
  filePointer = fopen(filename, "r");

  char asd[50] = "";
  // Loops through the lines
  while(fgets(buffer, buffer_size, filePointer)) {
    // check if line contains valid `id, id, cost` format
    char ori[20], dest[20], cost[20];
    if (!startswith(buffer, "//") && !startswith(buffer, "\n")) {
      // split buffer at delimiter and convert to int
      parse_line(buffer, " ", ori, dest, cost);
      // remove new line
      cost[strlen(cost) - 1] = 0;

      if (atoi(ori) == rid) {
        strcat(asd, dest);
        strcat(asd, " ");
        strcat(asd, cost);
        strcat(asd, " ");
      }
      else if(atoi(dest) == rid) {
        strncat(asd, ori, 2);
        strncat(asd, " ", 2);
        strncat(asd, cost, 3);
        strncat(asd, " ", 2);
      }
    }
  }
  fclose(filePointer);
  strcpy(v, asd);
}

// Set router data as port and ip address
int parse_router_config(char filename[], int rid, int *port, char ip[20]) {
  FILE* filePointer;
  int buffer_size = 255;
  char buffer[buffer_size];
  filePointer = fopen(filename, "r");

  int found = 0;
  while(fgets(buffer, buffer_size, filePointer)) {
    char id[20], temp_port[20], temp_ip[20];

    if (!startswith(buffer, "//") && !startswith(buffer, "\n")) {
      // split buffer at delimiter and convert to int
      parse_line(buffer, " ", id, temp_port, temp_ip);
      if (atoi(id) == rid) {
        *port = atoi(temp_port);
        // remove trailing new line
        temp_ip[strlen(temp_ip)-1] = '\0';
        strcpy(ip, temp_ip);
        found = 1;
        break;
      }
    }
  }

  fclose(filePointer);
  if (found == 0) {
    return -1;
  }
  return 0;
}
