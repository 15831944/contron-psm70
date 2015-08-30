#ifndef JK_WORKER_PROTOCOL_H
#define JK_WORKER_PROTOCOL_H

#include "station_def.h"
#include "jk_def.h"

void jk_client_heartbeat(stationclient_t *client, jk_command_t *command);
void jk_client_request_data_count(stationclient_t *client, jk_command_t *command);
void jk_client_request_data(stationclient_t *client, jk_command_t *command);
void jk_client_upload_device_state(stationclient_t *client, jk_command_t *command);
void jk_client_upload_device_lock_state(stationclient_t *client, jk_command_t *command);

void jk_client_request_device_unlock_lock(stationclient_t *client, jk_command_t *command);
void jk_client_request_unlock_all_device(stationclient_t *client, jk_command_t *command);
void jk_client_request_lock_all_device(stationclient_t *client, jk_command_t *command);

void jk_client_send_ticket(stationclient_t *client, jk_command_t *command);
void jk_client_request_sync_time(stationclient_t *client, jk_command_t *command);
void jk_client_upload_yk_device_state(stationclient_t *client, jk_command_t *command);

#endif // JK_WORKER_PROTOCOL_H
