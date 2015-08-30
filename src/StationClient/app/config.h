#ifndef CONFIG_H
#define CONFIG_H

#include "sys/platform.h"

#define IP_LEN 32
#define MAX_CLIENT 128
typedef struct _config_client
{
    int id;
    char ip[IP_LEN];
    int port;
} config_client_t;

typedef struct _config
{
    int rt_id;
    int rt_yk;
    int rt_ticket;
    char wf_server_ip[IP_LEN];
    int wf_server_port;

    int client_enable;
    char default_client_server_ip[IP_LEN];
    int default_client_server_port;
    int default_client_server_id;
    int client_count;
    int client_id[MAX_CLIENT];
    int client_server_id[MAX_CLIENT];
    char client_ip[MAX_CLIENT][IP_LEN];
    int client_port[MAX_CLIENT];

    int server_enable;
    int server_port;
    int server_station_no;
} config_t;

int config_start();
config_t *config_instance();

#endif // CONFIG_H
