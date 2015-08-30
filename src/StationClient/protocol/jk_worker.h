#ifndef JK_WORKER_H
#define JK_WORKER_H

#include "station_def.h"
#include "jk_def.h"

typedef struct _jk_worker
{
    void    *next;
    void    *target;
    void    *worker;
} jk_worker_t;

jk_worker_t *jk_worker_client_create(stationclient_t *client);
int jk_worker_client_handle(jk_worker_t *worker, jk_command_t *command);
void jk_worker_free(jk_worker_t *worker);

#endif // JK_WORKER_H
