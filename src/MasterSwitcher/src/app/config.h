#ifndef CONFIG_H
#define CONFIG_H

#define IP_LEN 16
#define NAME_LEN 255
typedef struct _config
{
    char FloatIP[IP_LEN];
    char FloatGateway[IP_LEN];
    char FloatNetmask[IP_LEN];
    char RemoteIP[IP_LEN];
    int HeartbeatPort;
    int ReconnectInterval;
    char LocalEthernet[NAME_LEN];
    char LocalGateway[IP_LEN];
    int SyncInterval;
} config_t;

config_t *loadConfig();

#endif // CONFIG_H
