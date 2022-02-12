int startswith(const char *original, const char *sufix);
void parse_line(char *buff, const char delim[1], char *ori, char *dest, char *cost);
void parse_enlaces_config(char filename[], int rid, char *v);
int parse_router_config(char filename[], int rid, int *port, char ip[20]);
