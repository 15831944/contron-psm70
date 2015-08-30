#ifndef PSM_H
#define PSM_H


#include "GroundServerInterface.h"
#include "app/global.h"
#include "app/config.h"
#include "app/ground.h"

typedef ground_t *(* gapp)();

int ground_psm_init(config_t *config);
int ground_psm_start(gapp app);
void ground_psm_free();
int ground_psm_unlock(unsigned int station, unsigned int controller, unsigned int id);
int ground_psm_lock(unsigned int station, unsigned int controller, unsigned int id);
int ground_psm_query(unsigned int station, unsigned int controller, unsigned int id);
int ground_psm_statereport(int station, int controller, int id, int state);



#endif // PSM_H
