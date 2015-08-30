#ifndef JK_CORE_H
#define JK_CORE_H

#include "sys/platform.h"

int jk_find_frame(char *buffer, int len, int &pos, int &frame_pos, int &frame_len);

USHORT jk_get_ushort(const char *data);
void jk_set_ushort(char **data, USHORT value);
void jk_set_uchar(char **data, UCHAR value);
void jk_set_buffer(char **data, char *buffer, int size);

#endif // JK_CORE_H
