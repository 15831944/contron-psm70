#include "psm_device.h"

bool device_same(device_t *device, char *name, int size)
{
    UN_USE(size);
    return (strlen(name)==strlen(device->name))
            &&(stricmp(device->name, name) == 0);
}

device_t *device_find(device_t *list, char *name, int size)
{
    device_t *result = NULL;

    if(NULL!=list)
    {
        device_t *next = list;
        while(next != NULL)
        {
            bool found = device_same(next, name, size);
            if(found)
            {
                result = next;
                break;
            }
            next = (device_t *)next->next;
        }
    }//if

    return result;
}

void device_append(device_t **list, device_t *d)
{
    device_t *n = *list;
    while((NULL!=n)&&(NULL!=n->next))
    {
        n = (device_t *)n->next;
    }
    if(NULL==n)
    {
        *list = d;
        return;
    }
    n->next = d;
}

void device_insert(device_t **list, char *name, int size)
{
    device_t *target = device_find(*list, name, size);
    if(NULL!=target)
    {
        return;
    }

    //未找到则创建
    device_t *d = (device_t *)malloc(sizeof(device_t));
    if(NULL==d)
    {
        return;
    }
    memset(d, 0, sizeof(device_t));
    memcpy(d->name, name, size);
    d->next = NULL;

    //加到队尾
    device_append(list, d);


    return;
}


int device_count(device_t *list)
{
    int device_count = 0;

    device_t *next = list;
    while(next != NULL)
    {
        device_count++;
        next = (device_t *)next->next;
    }
    return device_count;
}


void device_free(device_t **list)
{
    device_t *n = *list;
    while(NULL!=n)
    {
        device_t *t = n;
        n = (device_t *)n->next;
        free(t);
    }
    *list = NULL;
}


void device_remove(device_t **list, char *name, int size)
{
    if(NULL==list)
    {
        return;
    }

    device_t *next = *list;
    device_t *first = next;
    device_t *prev = NULL;
    while(next != NULL)
    {
        device_t *d = NULL;
        bool found = device_same(next, name, size);
        if(found)
        {
            d = next;
            if(d==first)
            {
                first = (device_t *)d->next;
                free(d);
                next = first;
                continue;
            }
            if(NULL!=prev)
            {
                prev->next = d->next;
                free(d);
                next = (device_t *)prev->next;
                continue;
            }
        }

        prev = next;
        next = (device_t *)next->next;
    }

    *list = first;
}
