#ifndef STATION_SERVER_H
#define STATION_SERVER_H

#include "sys/platform.h"
#include "station_def.h"

stationserver_t *stationserver_new();
void stationserver_start(stationserver_t *server);
void stationserver_append(stationclient_t **list, stationclient_t *client);
stationclient_t *stationserver_find(stationserver_t *server, int station_no, stationclient_t *client=NULL);
stationclient_t *stationserver_find_todo(stationserver_t *server, int station_no, int cmd, stationclient_t *client=NULL);
void stationserver_clear_todo(stationserver_t *server, stationclient_t *client, int cmd);

#endif // STATION_SERVER_H
