#include "jk_cmd_buffer.h"

void jk_cmd_buffer_append(jk_cmd_buffer_list_t *list, int cmd)
{
    if(jk_cmd_buffer_exist(list, cmd))
    {
        return;
    }
    int count = list->count;
    int size = (count+1)*(sizeof(jk_cmd_buffer_t));
    list->buffer = (jk_cmd_buffer_t *)realloc(list->buffer, size);
    if(NULL==list->buffer)
    {
        return;
    }
    jk_cmd_buffer_t *n = (jk_cmd_buffer_t *)(list->buffer+count);
    n->cmd = cmd;
    list->count = count+1;
}


int jk_cmd_buffer_exist(jk_cmd_buffer_list_t *list, int cmd)
{
    int result = NUMBER_FALSE;
    int count = list->count;
    for(int i=0;i<count;i++)
    {
        jk_cmd_buffer_t *t = (jk_cmd_buffer_t *)(list->buffer+i);
        if(cmd==t->cmd)
        {
            result = NUMBER_TRUE;
            break;
        }
    }
    return result;
}


void jk_cmd_buffer_clear(jk_cmd_buffer_list_t *list, int cmd)
{
    int count = list->count;
    for(int i=0;i<count;i++)
    {
        jk_cmd_buffer_t *t = (jk_cmd_buffer_t *)(list->buffer+i);
        if(cmd==t->cmd)
        {
            t->cmd = CMD_NONE;
        }
    }
}
