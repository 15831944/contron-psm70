#include "switchertestapp.h"

//#include "heartbeatprotocol.h"
//#include "heartbeat.h"
#include "sys/platform.h"

//void test()
//{
//    HeartbeatProtocol hp;
//    UINT32 i = 12345;
//    Heartbeat *hb = hp.make(i);
//    if(hb)
//    {
//        i = hb->getTimePoint();
//        printf("uint32=%d\n", i);
//        delete hb;
//    }
//}

void test1()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("%ld\t%ld\n", tv.tv_sec, tv.tv_usec);
    time_t t;
    t = tv.tv_sec;
    struct tm *p = gmtime(&t);

    char strTime[32];
    memset(strTime, 0, 32);

    strftime(strTime, 32, "%F %T", p);
    printf("%s\n", strTime);
}

int main()
{
//    test1();
//    return 0;

    SwitcherTestApp app;
    int ret = app.run();
    return ret;
}

