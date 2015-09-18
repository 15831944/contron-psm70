
#include "sys/platform.h"
#include "conn/tcp.h"

#include "masterswitcherapp.h"
//#include "heartbeat.h"

int main()
{
//    Heartbeat hb;
//    hb.setData(true, 1.23456);
//    printf("heartbeat:%d,%f\n", hb.getIsSlave(), hb.getTimePoint());
//    return 0;

    MasterSwitcherApp app;
    int res = app.run();
    return res;
}
