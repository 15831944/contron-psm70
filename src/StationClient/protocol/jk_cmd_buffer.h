#ifndef JK_CMD_BUFFER_H
#define JK_CMD_BUFFER_H

#include "sys/platform.h"

#define CMD_NONE                0x00
#define CMD_SYNC_TIME           0x01

typedef struct _jk_cmd_buffer
{
    int cmd;
} jk_cmd_buffer_t;

typedef struct _jk_cmd_buffer_list
{
    int             count;
    jk_cmd_buffer_t *buffer;
} jk_cmd_buffer_list_t;

int jk_cmd_buffer_exist(jk_cmd_buffer_list_t *list, int cmd);
void jk_cmd_buffer_append(jk_cmd_buffer_list_t *list, int cmd);
void jk_cmd_buffer_clear(jk_cmd_buffer_list_t *list, int cmd);

#endif // JK_CMD_BUFFER_H
