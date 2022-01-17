#include "string.h"

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

// Reads file line-by-line
void parse_file(char filename[], int rid) {
  FILE* filePointer;
  int buffer_size = 255;
  char buffer[buffer_size];
  filePointer = fopen(filename, "r");

  // Loops through the lines
  printf("\nReading file: %s\n", filename);
  while(fgets(buffer, buffer_size, filePointer)) {
    // check if line contains valid `id, id, cost` format
    char ori[20], dest[20], cost[20];

    if (!startswith(buffer, "//") && !startswith(buffer, "\n")) {
      // split buffer at delimiter and convert to int
      parse_line(buffer, " ", ori, dest, cost);
      if (atoi(ori) == rid || atoi(dest) == rid) {
        printf("%s %s %s", ori, dest, cost);
      }
    }
  }
  fclose(filePointer);
}

void parse_router_config(char filename[], int rid, int *port, char ip[20]) {
  FILE* filePointer;
  int buffer_size = 255;
  char buffer[buffer_size];
  filePointer = fopen(filename, "r");

  // Loops through the lines
  printf("\nReading file: %s\n", filename);
  printf("Searching for router #%d...\n", rid);
  int found = 0;

  while(fgets(buffer, buffer_size, filePointer)) {
    char id[20], temp_port[20], temp_ip[20];

    if (!startswith(buffer, "//") && !startswith(buffer, "\n")) {
      // split buffer at delimiter and convert to int
      parse_line(buffer, " ", id, temp_port, temp_ip);
      if (atoi(id) == rid) {
        *port = atoi(temp_port);
        strcpy(ip, temp_ip);
        printf("Found... port: %d ip: %d\n", atoi(temp_port), atoi(temp_ip));
        found = 1;
        break;
      }
    }
  }
  fclose(filePointer);
  if (found == 0) {
    printf("No data found for rounter `%d`\n", rid);
  }
}
