#ifndef CONFIG_H
#define CONFIG_H

#define IP_LEN 16
typedef struct _config
{
    char FloatIP[IP_LEN];
    char FloatGateway[IP_LEN];
    char RemoteIP[IP_LEN];
    int HeartbeatPort;
    int ReconnectInterval;
} config_t;

config_t *config_instance();

#endif // CONFIG_H
