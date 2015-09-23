#include "sync.h"

#include "log.h"

#if WIN32
#define START_SYNC_SCRIPT "../scripts/start_sync.bat"
#define START_SYNC_COMMAND \
    "call "START_SYNC_SCRIPT
#else
#define START_SYNC_SCRIPT "../scripts/start_sync.sh"
#define START_SYNC_COMMAND \
    ". "START_SYNC_SCRIPT
#endif

#if WIN32
#define SYNC_SCRIPT "../scripts/sync.bat"
#define SYNC_COMMAND \
    "call "SYNC_SCRIPT
#else
#define SYNC_SCRIPT "../scripts/sync.sh"
#define SYNC_COMMAND \
    ". "SYNC_SCRIPT
#endif

#if WIN32
#define STOP_SYNC_SCRIPT "../scripts/stop_sync.bat"
#define STOP_SYNC_COMMAND \
    "call "STOP_SYNC_SCRIPT
#else
#define STOP_SYNC_SCRIPT "../scripts/stop_sync.sh"
#define STOP_SYNC_COMMAND \
    ". "STOP_SYNC_SCRIPT
#endif

Sync::Sync()
    : OnlineChecker()
{
    mEnable = false;
}

Sync::~Sync()
{

}

void Sync::checkOnline()
{
    bool enable = false;
    enter();
    enable = mEnable;
    leave();

    if(!enable)
    {
        return;
    }

    execSyncScript();
}

void Sync::enableSync()
{
    enter();
    mEnable = true;
    leave();

    execStartSyncScript();
    execSyncScript();
}

void Sync::disableSync()
{
    enter();
    mEnable = false;
    leave();

    execStopSyncScript();
}

void Sync::execStartSyncScript()
{
    APP_LOG("[Sync]start sync \n");
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, START_SYNC_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return;
    }

}

void Sync::execSyncScript()
{
    APP_LOG("[Sync]on sync:begin \n");
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, SYNC_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    APP_LOG("[Sync]on sync:end \n");
    if(1==ret)
    {
        return;
    }
}

void Sync::execStopSyncScript()
{
    APP_LOG("[Sync]stop sync \n");
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, STOP_SYNC_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return;
    }
}

