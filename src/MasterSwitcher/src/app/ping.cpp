#include "ping.h"

#include "sys/platform.h"

#if WIN32
#define PING_SCRIPT "../scripts/ping.bat"
#define PING_COMMAND \
    "call "PING_SCRIPT" %s %d"
#else
#define PING_SCRIPT "../scripts/ping.sh"
#define PING_COMMAND \
    ". "PING_SCRIPT" %s %d"
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
    sprintf(command, PING_COMMAND, ip, count);
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

