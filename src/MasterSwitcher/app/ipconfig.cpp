#include "ipconfig.h"

#include "sys/platform.h"

#if WIN32
#define IP_SCRIPT "scripts/ip.bat"
#define IP_COMMAND \
    "call "IP_SCRIPT" %s %s %s"
#else
#define IP_SCRIPT "scripts/ip.sh"
#define IP_COMMAND \
    ". "IP_SCRIPT" %s %s %s %s %s %s"
#endif

Ipconfig::Ipconfig()
{

}

Ipconfig::~Ipconfig()
{

}

bool Ipconfig::addIP(char *ethernet, char *ip, char *netmask, char *gateway, char *broadcast)
{
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, IP_COMMAND, "add", ip, ethernet, netmask, gateway, broadcast);
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

bool Ipconfig::delIP(char *ethernet, char *ip)
{
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, IP_COMMAND, "del", ip, ethernet);
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

bool Ipconfig::hasIP(char *ip)
{
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, IP_COMMAND, "search", ip);
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

