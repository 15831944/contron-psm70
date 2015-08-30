#ifndef INTF_H
#define INTF_H

#include "global.h"

typedef int (*statereport)(int,int,int,int);
typedef struct
{
    statereport statereport_immediate;
    statereport statereport_interval;
} intf_t;

intf_t *ground_intf();
void ground_intf_free();

void ground_intf_statereport_immediate_set(statereport func);
void ground_intf_statereport_interval_set(statereport func);

#endif // INTF_H
