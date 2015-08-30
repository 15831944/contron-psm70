#include "app_intf.h"

int inited = NUMBER_FALSE;
app_intf_t single;

app_intf_t *app_intf_instance()
{
    if(!inited)
    {
        memset(&single, 0, sizeof(app_intf_t));
    }
    return &single;
}
