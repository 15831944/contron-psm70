#include "log.h"

#include "sys/platform.h"

#define MAXLINESIZE 10240
#define MAXLOGSIZE 1024*1024*10  //单个日志文件最大10M
CRITICAL_SECTION log_critical;
char logfile[] = "ground.log";
char logbakfile[] = "ground.log.1";
FILE *flog;
static char logstr[MAXLINESIZE+1];

void logV(const char *fmt, va_list argp)
{
    if(NULL==fmt||0==fmt[0])
    {
        return;
    }
    vsnprintf(logstr, MAXLINESIZE, fmt, argp);
    struct tm *now;
    struct timeb tb;
    ftime(&tb);
    now = localtime(&tb.time);
    char datestr[16];
    char timestr[16];
    sprintf(datestr, "%04d-%02d-%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
    sprintf(timestr, "%02d:%02d:%02d:%03d", now->tm_hour, now->tm_min, now->tm_sec, tb.millitm);

    flog = fopen(logfile, "a");
    if(NULL!=flog)
    {
        fprintf(flog, "%s %s %s", datestr, timestr, logstr);
        int pos = ftell(flog);
        fclose(flog);
        //备份日志文件到.1文件
        bool backup = (pos>MAXLOGSIZE);
        if(backup)
        {
            remove(logbakfile);
            bool can_rename = (0==rename(logfile, logbakfile));
            if(can_rename)
            {
                remove(logfile);
            }
        }
    }
}

void ground_log(const char *fmt, ...)
{
    va_list argp;
    ground_log_enter();
    va_start(argp, fmt);
    logV(fmt, argp);
    va_end(argp);
    ground_log_leave();
}


void ground_log_init()
{
    MUTEX_INIT(&log_critical);
}


void ground_log_free()
{
    MUTEX_FREE(&log_critical);
}


void ground_log_enter()
{
    MUTEX_LOCK(&log_critical);
}


void ground_log_leave()
{
    MUTEX_UNLOCK(&log_critical);
}
