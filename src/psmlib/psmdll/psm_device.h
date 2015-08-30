#ifndef PSM_DEVICE_H
#define PSM_DEVICE_H

#include "sys/platform.h"

#define MAX_DEVICE_LEN 64
/*!
  目前PSM系统中主编号长度为16
 */
#define ID_SIZE 16

typedef struct _device
{
    void    *next;
    char    name[MAX_DEVICE_LEN];
    UINT32  inner_id;
    UCHAR   value;
    UCHAR   lock_value;
    void    *extend;
}device_t;


device_t *device_find(device_t *list, char *name, int size);

void device_append(device_t **list, device_t *d);

void device_insert(device_t **list, char *name, int size);

void device_remove(device_t **list, char *name, int size);

int device_count(device_t *list);

void device_free(device_t **list);



#endif // PSM_DEVICE_H
