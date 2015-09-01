#include "ipconfig.h"

#include "sys/platform.h"

#if WIN32
#define IP_SCRIPT "../scripts/ip.bat"
#define IP_COMMAND \
    "call "IP_SCRIPT" %s %s %s %s %s %s"
#else
#define IP_SCRIPT "../scripts/ip.sh"
#define IP_COMMAND \
    ". "IP_SCRIPT" %s %s %s %s %s %s"
#endif

Ipconfig::Ipconfig()
{

}

Ipconfig::~Ipconfig()
{

}

bool Ipconfig::addIP(char *ethernet, char *ip, const char *netmask, const char *gateway, const char *broadcast)
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
    sprintf(command, IP_COMMAND, "del", ip, ethernet, "", "", "");
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

    struct tm *now;
    struct timeb tb;
    ftime(&tb);
    now = localtime(&tb.time);
    char datestr[16];
    char timestr[16];
    sprintf(datestr, "%04d_%02d_%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
    sprintf(timestr, "%02d_%02d_%02d_%03d", now->tm_hour, now->tm_min, now->tm_sec, tb.millitm);
    char filestr[36];
    memset(filestr, 0, sizeof(filestr));
    sprintf(filestr, "%s_%s", datestr, timestr);

    sprintf(command, IP_COMMAND, "search", ip, filestr, "", "", "");
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

