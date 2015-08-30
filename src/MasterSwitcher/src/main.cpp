
#include "sys/platform.h"
#include "conn/tcp.h"

#include "masterswitcherapp.h"

int main()
{
    MasterSwitcherApp app;
    int res = app.run();
    return res;
}
