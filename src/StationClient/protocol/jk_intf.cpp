#include "jk_intf.h"

static int inited = NUMBER_FALSE;
static jk_intf_t self;

jk_intf_t *jk_intf_instance()
{
    if(!inited)
    {
        memset(&self, 0, sizeof(jk_intf_t));
        inited = NUMBER_TRUE;
    }

    return &self;
}
