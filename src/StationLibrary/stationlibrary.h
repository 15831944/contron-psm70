#ifndef STATIONLIBRARY_H
#define STATIONLIBRARY_H

#include "stationlibrary_global.h"

#include "psm_def.h"


typedef void (*def_log)(char *);
//typedef void (*psm_request_data)(int);
typedef void (*device_unlock_result)(int, int, char *, int, int);
//typedef void (*psm_data_change)(int, jk_device_list_t *);
typedef struct _dll
{
    def_log log_function;
    device_unlock_result device_unlock_result_function;
    psm_request_data    main2jk_request_data;
    psm_request_data    sub2jk_request_data;
    psm_device_list     server_data_change;
    psm_device_list     server_lock_data_change;
    psm_device_list     client_data_change;
    psm_device_list     client_lock_data_change;
    psm_device_list     app_receive_ticket;
    psm_device_list     app_unlock_device;
    psm_device_list     app_lock_device;
    psm_device_list     client_unlock_device_result;
    psm_device_list     client_lock_device_result;
    psm_request_data    main2jk_send_request_sync_time;
    psm_request_data    sub2jk_send_request_sync_time;
    psm_request_data    mainstation_request_sync_time;
    psm_request_data    substation_request_sync_time;
    psm_sync_time       jk2main_reply_sync_time;
    psm_sync_time       jk2sub_reply_sync_time;
    psm_sync_time       main2jk_send_sync_time;
    psm_sync_time       sub2jk_send_sync_time;
    psm_device_list     main2jk_send_ticket;
    psm_device_list     jk2main_yk_device_state;
    psm_device_list     sub2jk_send_yk_device_state;
    psm_device_list     main2jk_send_lock_device;
    psm_device_list     main2jk_send_unlock_device;
    psm_device_list     jk2main_unlock_device_result;
    psm_device_list     jk2main_lock_device_result;
    psm_request_data    main2jk_send_lock_all_device;
    psm_request_data    main2jk_send_unlock_all_device;
    psm_request_data    jk2sub_lock_all_device;
    psm_request_data    jk2sub_unlock_all_device;
    psm_device_all      sub2jk_send_lock_device_all_result;
    psm_device_all      sub2jk_send_unlock_device_all_result;
    psm_device_all      jk2main_lock_device_all_result;
    psm_device_all      jk2main_unlock_device_all_result;
    int value;
    int mode;
} dll_t;


#define MODE_SERVER         0
#define MODE_SUBSTATION     1
#define MODE_MAINSTATION    2

void STATIONLIBRARYSHARED_EXPORT dll_init(int mode=0);
//server function
void STATIONLIBRARYSHARED_EXPORT set_main2jk_request_data(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_request_data(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_server_data_change(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_client_data_change(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_server_lock_data_change(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_client_lock_data_change(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_unlock_device_result(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_lock_device_result(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_request_sync_time(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_request_sync_time(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_reply_sync_time(psm_sync_time fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_reply_sync_time(psm_sync_time fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_ticket(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_yk_device_state(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_lock_device(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_unlock_device(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_device_unlock_result(device_unlock_result fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_lock_all_device(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_main2jk_send_unlock_all_device(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_lock_all_device_result(psm_device_all fnn);
void STATIONLIBRARYSHARED_EXPORT set_sub2jk_send_unlock_all_device_result(psm_device_all fnn);

void STATIONLIBRARYSHARED_EXPORT jk2sub_unlock_device(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2sub_lock_device(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2sub_reply_sync_time(int station_no, jk_time_t *time);
void STATIONLIBRARYSHARED_EXPORT jk2main_reply_sync_time(int station_no, jk_time_t *time);
void STATIONLIBRARYSHARED_EXPORT jk2sub_request_sync_time(int station_no);
void STATIONLIBRARYSHARED_EXPORT jk2main_request_sync_time(int station_no);
void STATIONLIBRARYSHARED_EXPORT jk2sub_send_ticket(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2main_yk_device_state(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2main_lock_device_result(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2main_unlock_device_result(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT jk2sub_lock_all_device(int station_no);
void STATIONLIBRARYSHARED_EXPORT jk2sub_unlock_all_device(int station_no);
void STATIONLIBRARYSHARED_EXPORT jk2main_lock_all_device_result(int station_no, int result);
void STATIONLIBRARYSHARED_EXPORT jk2main_unlock_all_device_result(int station_no, int result);


//substation
void STATIONLIBRARYSHARED_EXPORT set_substation_receive_ticket(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_request_sync_time(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_sync_time(psm_sync_time fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_unlock_device(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_lock_device(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_lock_all_device(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_substation_unlock_all_device(psm_request_data fnn);

void STATIONLIBRARYSHARED_EXPORT app_send_yk_device_state(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_send_unlock_device_result(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_send_lock_device_result(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_send_request_sync_time(int station_no);
void STATIONLIBRARYSHARED_EXPORT app_send_lock_device_all_result(int station_no, int result);
void STATIONLIBRARYSHARED_EXPORT app_send_unlock_device_all_result(int station_no, int result);

//mainstation
void STATIONLIBRARYSHARED_EXPORT set_mainstation_request_sync_time(psm_request_data fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_sync_time(psm_sync_time fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_yk_device_state(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_lock_device_result(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_unlock_device_result(psm_device_list fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_lock_device_all_result(psm_device_all fnn);
void STATIONLIBRARYSHARED_EXPORT set_mainstation_unlock_device_all_result(psm_device_all fnn);

void STATIONLIBRARYSHARED_EXPORT app_send_lock_device(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_send_unlock_device(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_send_lock_device_all(int station_no);
void STATIONLIBRARYSHARED_EXPORT app_send_unlock_device_all(int station_no);
void STATIONLIBRARYSHARED_EXPORT app_send_reply_sync_time(int station_no, jk_time_t *time);
void STATIONLIBRARYSHARED_EXPORT app_send_ticket(int station_no, jk_device_list_t *devices);


void STATIONLIBRARYSHARED_EXPORT client_device_unlock(int station_no, int device_len, char *device, int operate);
void STATIONLIBRARYSHARED_EXPORT set_value(int value);
int STATIONLIBRARYSHARED_EXPORT get_value();
void STATIONLIBRARYSHARED_EXPORT set_log(def_log fnn);

void STATIONLIBRARYSHARED_EXPORT app_request_data(int station_no);
void STATIONLIBRARYSHARED_EXPORT app_data_change(int station_no, jk_device_list_t *devices);
void STATIONLIBRARYSHARED_EXPORT app_lock_data_change(int station_no, jk_device_list_t *devices);


#endif // STATIONLIBRARY_H
