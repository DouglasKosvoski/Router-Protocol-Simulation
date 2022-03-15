/*
 * load_config.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Check if string starts with a specific sufix
int startswith(const char *original, const char *sufix);

// Responsible for spliting a buffer at a delimiter and converting char[] into an integer
void parse_line(char *buff, const char delim[1], char *ori, char *dest, char *cost);

// Go through enlaces.config file and search for line with router id, then get its content
void parse_enlaces_config(char filename[], int rid, char *v);

// Set router data as port and ip address
int parse_router_config(char filename[], int rid, int *port, char ip[20]);

// Simply clear the terminal
void clear_terminal();

// Check if router id was passed on execution
int check_arguments(int args);

void get_timestamp(char *tt);