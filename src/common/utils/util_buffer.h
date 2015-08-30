#ifndef UTIL_BUFFER_H
#define UTIL_BUFFER_H

//#include "sys/platform.h"


//#ifdef  __cplusplus
//extern "C" {
//#endif

/*!
  数据项
 */
typedef struct _data
{
    int         size;               //实际数据大小，小于等于DATA_SIZE
    char        *data;              //数据存储区
    void        *next;
} data_t;

/*!
  数据列表
 */
typedef struct _data_list
{
    data_t        *data;
    void        *next;
} data_list_t;

char *buffer_format(char *buffer, int size);

int data_append(data_t **list, char *buffer, int size);
void data_free(data_t **list);
void data_remove(data_t **list, data_t *data);


//#ifdef  __cplusplus
//}
//#endif

#endif // UTIL_BUFFER_H
