#include "util_buffer.h"

#include "sys/platform.h"

/*!
 * \brief buffer_format     格式化内存数据
 * \param buffer            内存区域
 * \param size              内存区域大小
 * \return                  格式化的字符串，用十六进制显示字节，第个字节用空格格开
 */
char *buffer_format(char *buffer, int size)
{
    static char str[MAX_STRING] = {0};
    char *p = str;
    for(int i=0;i<size;i++)
    {
        sprintf(p, "%02X ", (UCHAR)(*(buffer+i)));
        p+=3;
    }
    *(p-1) = 0;
    return str;
}

/*!
 * \brief data_create       从数据区创建数据项
 * \param buffer            数据区指针
 * \param size              数据区大小
 * \return                  成功则返回包含数据区内容的数据项，否则返回NULL
 * \note                    在内存中为数据项分配空间，再将数据区中数据拷贝过去
 */
data_t *data_create(char *buffer, int size)
{
    data_t *result = (data_t *)malloc(sizeof(data_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(data_t));
        result->data = (char *)malloc(size);
        if(NULL==result->data)
        {
            free(result);
            result = NULL;
        }
        else
        {
            result->size = size;
            memcpy(result->data, buffer, size);
        }
    }
    return result;
}

void data_insert(data_t **list, data_t *d)
{
    data_t *n = *list;
    while((NULL!=n)&&(NULL!=n->next))
    {
        n = (data_t *)n->next;
    }
    if(NULL==n)
    {
        *list = d;
        return;
    }
    n->next = d;
}

int data_append(data_t **list, char *buffer, int size)
{
    if(NULL==buffer || 0==size)
    {
        return NUMBER_FALSE;
    }
    data_t *t = data_create(buffer, size);
    if(NULL==t)
    {
        return NUMBER_FALSE;
    }
    data_insert(list, t);
    return NUMBER_TRUE;
}


void data_free(data_t **list)
{
    data_t *n = *list;
    while(NULL!=n)
    {
        data_t *t = n;
        n = (data_t *)n->next;
        if(0<t->size)
        {
            free(t->data);
        }
        free(t);
    }
    *list = NULL;
}


void data_remove(data_t **list, data_t *data)
{
    data_t *n = *list;
    if(NULL==n)
    {
        return;
    }
    if(n==data)
    {
        *list = (data_t *)n->next;
        n->next = NULL;
        data_free(&n);
        return;
    }
    while(NULL!=n)
    {
        data_t *t = (data_t *)n->next;
        if(t==data)
        {
            n->next = (NULL==t)?NULL:t->next;
            if(NULL!=t)
            {
                t->next = NULL;
                data_free(&t);
            }
        }
        n = (data_t *)n->next;
    }
}
