#ifndef CONFIG_H
#define CONFIG_H

#define IP_LEN 16
#define NAME_LEN 255
typedef struct _config
{
    int IsServer;
    int ServerPort;
    char ServerIP[IP_LEN];
    int ReconnectInterval;
} config_t;

config_t *loadConfig();

#endif // CONFIG_H
