#include "string.h"

// Check if string starts with a specific sufix
int startswith(const char *original, const char *sufix) {
   if(strncmp(original, sufix, strlen(sufix)) == 0) {
     return 1;
   };
   return 0;
}

// Responsible for spliting a buffer at a delimiter and converting char[] into an integer
void parse_line(char *buff, const char delim[1], int *ori, int *dest, int *cost) {
  char *token = strtok(buff, delim);
  int counter = 0;
  while(token != NULL) {
    if (counter == 0) {
      *ori = atoi(token);
    }
    else if (counter == 1) {
      *dest = atoi(token);
    }
    else if (counter == 2) {
      *cost = atoi(token);
    }
    counter++;
    token = strtok(NULL, delim);
  }
}

// Reads file line-by-line
void parse_file(char filename[]) {
  FILE* filePointer;
  int buffer_size = 255;
  char buffer[buffer_size];
  filePointer = fopen(filename, "r");

  // Loops through the lines
  printf("\nReading file: %s\n", filename);
  while(fgets(buffer, buffer_size, filePointer)) {
    // check if line contains valid `id, id, cost` format
    int origem = -1, destino = -1, custo = -1;
    if (!startswith(buffer, "//") && !startswith(buffer, "\n")) {
      // split buffer at delimiter and convert to int
      parse_line(buffer, " ", &origem, &destino, &custo);
      printf("%d %d %d\n", origem, destino, custo);
    }
  }
  fclose(filePointer);
}
