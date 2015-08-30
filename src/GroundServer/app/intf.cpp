#include "intf.h"


static intf_t *self = NULL;

intf_t *ground_intf()
{
    if(NULL==self)
    {
        self = (intf_t *)malloc(sizeof(intf_t));
        memset(self, 0, sizeof(intf_t));
    }
    return self;
}


void ground_intf_free()
{
    if(NULL!=self)
    {
        free(self);
    }
    self = NULL;
}


void ground_intf_statereport_immediate_set(statereport func)
{
    ground_intf()->statereport_immediate = func;
}


void ground_intf_statereport_interval_set(statereport func)
{
    ground_intf()->statereport_interval = func;
}
