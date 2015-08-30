#ifndef JK_WORKER_COMMON_H
#define JK_WORKER_COMMON_H

#include "station_def.h"
#include "jk_def.h"

int jk_command_check(jk_command_t *command, UCHAR cmd, UCHAR fnn);
int jk_command_check_cmd(jk_command_t *command, UCHAR cmd);
int jk_command_check_fnn(jk_command_t *command, UCHAR fnn);
int jk_command_check_back(jk_command_t *command, UCHAR cmd);
int jk_command_check_data_struct(jk_command_t *command);
int jk_client_command_valid(stationclient_t *client, jk_command_t *command);

#endif // JK_WORKER_COMMON_H
