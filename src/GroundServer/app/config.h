#ifndef configIG_H
#define configIG_H

#include "controller.h"
#include "global.h"

/*
 * 系统配置信息
 */
typedef struct
{
    int state_interval;  //地线状态汇报时间间隔，单位：秒
    int controller_count;
    controller_t controller[MAX_CONTROLLER];
} config_t;


//int ground_config_init(config_t *config);
//int ground_config_loadfile(config_t *config, char *file);
//int ground_config_savefile(config_t *config, char *file);

#endif // configIG_H
