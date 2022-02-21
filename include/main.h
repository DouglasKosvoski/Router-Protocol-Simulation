/* Configuration files */
#define enlaces_cfg "./cfg/enlaces.config"
#define roteador_cfg "./cfg/roteador.config"

void send_distance_vector();
void set_router(const char *i);
void display_router_info();
void display_neighbours_info();
void serialize_message(char *m, Message *msg);
void deserialize_msg(Message *msg, char *serialized_msg);
void get_user_message();
void *terminal(void *);
void *packet_handler(void *);
void *sender(void *);
void *receiver(void *);