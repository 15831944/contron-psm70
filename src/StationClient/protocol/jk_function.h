#ifndef JKFUNCTION_H
#define JKFUNCTION_H

#include "sys/platform.h"
#include "jk_def.h"
#include "jk_core.h"
#include "station_def.h"
#include "utils/utils.h"

typedef struct _buffer
{
    int size;
    char *data;
} buffer_t;


typedef struct
{
    int station_no;
    void *next;
} queue_t;

typedef struct
{
    queue_t *send_queue;
} jk_t;

//void jk_find_frame(const buffer_t *buf, frame_t *f, buffer_t *remain);
command_t *jk_command_build(char *frame);
command_t *jk_command_ack(const command_t *cmd, UCHAR error_no = 0);
command_t *jk_command_new();

//void jk_sync_time_request(int station_no);
//void jk_sync_time_response(bool success, int station_no, time_t *server_time);

//void jk_ticket_print_number_request();
//void jk_ticket_print_number_response();
//void jk_ticket_number_printed();

extern char *buffer_format(char *buffer, int size);
extern void log(const char *fmt, ...);
extern void protocol_client_send(stationclient_t *client, char *buffer, int size);

void jk_command_heartbeat(stationclient_t *client);
data_t *jk_command_buffer(jk_command_t *cmd);
void jk_command_free(jk_command_t **cmd);
void jk_command_send(stationclient_t *client, jk_command_t *command);
void jk_client_receive(stationclient_t *client, char *buffer, int len);
void jk_request_data_count(stationclient_t *client);
void jk_server_request_data_count(stationclient_t *client);

void jk_send_device_unlock_result(stationclient_t *client, jk_device_list_t *devices);
void jk_send_device_lock_result(stationclient_t *client, jk_device_list_t *devices);
void jk_server_device_unlock(stationclient_t *client, int device_len, char *device, int operate);
void jk_client_device_state_upload(stationclient_t *client, int device_len, char *device, int state);
void jk_client_device_lock_state_upload(stationclient_t *client, int device_len, char *device, int state);
void jk_request_sync_time(stationclient_t *client);
void jk_reply_sync_time(stationclient_t *client, jk_time_t *time);
void jk_send_ticket(stationclient_t *client, jk_device_list_t *devices);
void jk_send_yk_device_state(stationclient_t *client, jk_device_list_t *devices);
void jk_send_lock_device(stationclient_t *client, jk_device_list_t *devices);
void jk_send_unlock_device(stationclient_t *client, jk_device_list_t *devices);
void jk_send_lock_all_device(stationclient_t *client);
void jk_send_unlock_all_device(stationclient_t *client);
void jk_send_lock_all_device_result(stationclient_t *client, int result);
void jk_send_unlock_all_device_result(stationclient_t *client, int result);
void jk_send_kbq(stationclient_t *client, jk_device_list_t *devices);

#endif // JKFUNCTION_H
