#include "ping.h"

#include "sys/platform.h"

#if WIN32
#define PING_SCRIPT "../scripts/ping.bat"
#define PING_COMMAND \
    "call "PING_SCRIPT" %s %d %s"
#else
#define PING_SCRIPT "../scripts/ping.sh"
#define PING_COMMAND \
    ". "PING_SCRIPT" %s %d %s"
#endif

Ping::Ping()
{

}

Ping::~Ping()
{

}

bool Ping::exec(char *ip, int count)
{
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));

    struct tm *now;
    struct timeb tb;
    ftime(&tb);
    now = localtime(&tb.time);
    char datestr[16];
    char timestr[16];
    sprintf(datestr, "%04d_%02d_%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
    sprintf(timestr, "%02d_%02d_%02d_%03d", now->tm_hour, now->tm_min, now->tm_sec, tb.millitm);
    char filestr[64];
    memset(filestr, 0, sizeof(filestr));
    sprintf(filestr, "ping_%s_%s", datestr, timestr);

    sprintf(command, PING_COMMAND, ip, count, filestr);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
      return false;
    while(fgets(buffer, 128, f))
    {
       printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return true;
    }
    return false;
}

