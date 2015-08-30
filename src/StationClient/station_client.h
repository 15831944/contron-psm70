#ifndef STATIONCLIENT_H
#define STATIONCLIENT_H

#include "station_def.h"


stationclient_t *stationclient_new();

void stationclient_start(stationclient_t *client);
void stationclient_close(stationclient_t *client);
void stationclient_enter(stationclient_t *client);
void stationclient_leave(stationclient_t *client);
void stationclient_disconnect(stationclient_t *client, bool reconnect_immediate);
void stationclient_connect(stationclient_t *client);
void stationclient_break(stationclient_t *client);

void stationclient_server(stationclient_t *client);
void stationclient_heartbeat_update(stationclient_t *client, int update_mode, USHORT station_no);

stationclient_t *stationclient_find(stationclient_t *client, int station_no);
bool stationclient_in_server(stationclient_t *client);
int stationclient_id(stationclient_t *client);
void stationclient_todo_cmd(stationclient_t *client, int cmd);

#endif // STATIONCLIENT_H
