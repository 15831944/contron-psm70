#ifndef PSMSYS_H
#define PSMSYS_H

#include "psm_def.h"

int psm_start(psm_config_t *config);

device_t *psm_device_get(int station_no);
jk_device_list_t *psm_get_device(int station_no);
int psm_device_count(int station_no);
void psm_device_update_state(int station, int device_len, char *device, int state);
void psm_device_update_lock_state(int station, int device_len, char *device, int state);
void psm_clear_lock_device(int station_no);
void psm_fix_lock_device(int station_no, jk_device_list_t list);

int hello_psm_1();

int psm_get_kbq(int station_no, jk_device_list_t *list);
int psm_receive_tikcet(int station_no, jk_device_list_t *list);
int psm_device_in_yk_table(int station_no, char *device, int device_len, int mode);
int psm_device_can_operate(int station_no, char *device, int device_len, int operate);
int psm_device_operate(int station_no, char *device, int device_len, int operate);
#endif // PSMSYS_H
