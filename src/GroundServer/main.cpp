#include <iostream>

using namespace std;

#include "ground.h"
#include "log.h"
#include "intf.h"

#ifdef HAS_PSM
#if WIN32 && (_MSC_VER>=1500)
#pragma comment(lib, "GroundServer.lib")
#endif
#include "psm.cpp"
#define ERROR_PSM_INIT -1
#define ERROR_PSM_START -1
#endif

int main()
{
    cout << "Welcome to Ground Server of PSM70!" << endl;
    ground_log_init();
    tcp_init();
    config_t config[1] = {};
    config_t *cfg = &config[0];

#ifdef HAS_PSM
    if(NUMBER_ERROR==ground_psm_init(cfg))
    {
        return ERROR_PSM_INIT;
    }
    cfg->state_interval = 15;
#else
///*
    char caption[MAX_STRING];
    cfg->state_interval = 15;
    cfg->controller_count = 1;
    cfg->controller[0].contronller_no = 1;
    cfg->controller[0].device_count = 1;
    sprintf(caption, "%s", "dx001");
    ground_device_init(cfg->controller[0].device,
            1, caption, 0);
//    sprintf(caption, "%s", "dx002");
//    ground_device_init(&cfg->controller[1].device[1],
//            2, caption, 7);
    sprintf(cfg->controller[0].conn[0].tcp[0].hostname, "%s", "10.7.5.22");
    cfg->controller[0].conn[0].tcp[0].port = 4001;
//*/

//    ground_conifg_savefile(config);
//    if(!ground_config_init(config))
//    {
//        return 1;
//    }
#endif
    ground_t *ground = ground_new(config);
    if(NULL==ground)
    {
        return 1;
    }

#ifdef HAS_PSM
    if(NUMBER_ERROR==ground_psm_start(ground_app))
    {
        return ERROR_PSM_START;
    }

    ground_intf_statereport_immediate_set(ground_psm_statereport);
    ground_intf_statereport_interval_set(ground_psm_statereport);    
#endif

    ground_start(ground);
#ifdef HAS_PSM
    ground_psm_free();
#endif
    tcp_free();
    ground_log_free();
    return 0;
}

