#ifndef TCP_H
#define TCP_H

#include "sys/platform.h"


#ifdef  __cplusplus
extern "C" {
#endif

typedef struct
{
    char hostname[16];
    int port;
    int local_if;

    SOCKET_HANDLE fd;

    pthread_t receive_thread[1];

    char *buffer;
    int buffer_size;
} tcp_t;


void tcp_init();
void tcp_free();
void tcp_config(tcp_t *tcp, char *hostname, int port);
void tcp_connect(tcp_t *tcp);
void tcp_close(tcp_t *tcp);
int tcp_isvalid(tcp_t *tcp);
int tcp_send(tcp_t *tcp, char *data, int size);
void tcp_zero(tcp_t *tcp);

#ifdef  __cplusplus
}
#endif

#endif // TCP_H
