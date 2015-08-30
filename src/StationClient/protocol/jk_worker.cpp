#include "jk_worker.h"

#include "jk_worker_protocol.h"

jk_worker_t *jk_worker_create()
{
    jk_worker_t *result = (jk_worker_t *)malloc(sizeof(jk_worker_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(jk_worker_t));
    }
    return result;
}

jk_worker_t *jk_worker_client_create(stationclient_t *client)
{
    jk_worker_t *result = NULL;
    jk_worker_t *work = jk_worker_create();
    if(NULL!=work)
    {
        work->target = client;
        work->worker = (void *)jk_client_heartbeat;
        result = work;
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_data_count;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_data;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_upload_device_state;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_upload_device_lock_state;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_device_unlock_lock;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_unlock_all_device;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_lock_all_device;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_send_ticket;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_request_sync_time;
            work->next = w;
            work = w;
        }
    }
    {
        jk_worker_t *w = jk_worker_create();
        if(NULL!=w)
        {
            w->target = client;
            w->worker = (void *)jk_client_upload_yk_device_state;
            work->next = w;
            work = w;
        }
    }

    return result;
}


typedef int (*client_function)(stationclient_t *, jk_command_t *);
int jk_worker_client_handle(jk_worker_t *worker, jk_command_t *command)
{
    client_function func = (client_function)worker->worker;
    stationclient_t *client = (stationclient_t *)worker->target;
    return func(client, command);
}


void jk_worker_free(jk_worker_t *worker)
{
    jk_worker_t *next = worker;
    while(NULL!=next)
    {
        jk_worker_t *t = next;
        next = (jk_worker_t *)next->next;
        free(t);
    }
}
