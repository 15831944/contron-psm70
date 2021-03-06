#include "log.h"

#include "sys/platform.h"

#define MAXLINESIZE 10240
#define MAXLOGSIZE 1024*1024*10  //单个日志文件最大10M
CRITICAL_SECTION log_critical;
char logfile[] = "../log/app.log";
char logbakfile[] = "../log/app.log.1";
FILE *flog;
static char logstr[MAXLINESIZE+1];

#if WIN32
#define LOG_SCRIPT "../scripts/log.bat"
#define LOG_COMMAND \
    "call "LOG_SCRIPT" %s \"""%s\""
#else
#define LOG_SCRIPT "../scripts/log.sh"
#define LOG_COMMAND \
    ". "LOG_SCRIPT" %s \"""%s\""
#endif

void logV(const char *fmt, va_list argp)
{
    if(NULL==fmt||0==fmt[0])
    {
        return;
    }
    memset(logstr, 0, MAXLINESIZE+1);
    vsnprintf(logstr, MAXLINESIZE, fmt, argp);
    struct tm *now;
    struct timeb tb;
    ftime(&tb);
    now = localtime(&tb.time);
    char datestr[16];
    char timestr[16];
    sprintf(datestr, "%04d-%02d-%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
    sprintf(timestr, "%02d:%02d:%02d:%03d", now->tm_hour, now->tm_min, now->tm_sec, tb.millitm);

    printf("%s\t%s\t%s\n", datestr, timestr, logstr);

log_enter();
    flog = fopen(logfile, "a");
    int pos=0;
    if(NULL!=flog)
    {
        fprintf(flog, "%s %s %s", datestr, timestr, logstr);
        pos = ftell(flog);
        fclose(flog);
    }
log_leave();

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

void _log(const char *fmt, ...)
{
    va_list argp;
//    log_enter();
    va_start(argp, fmt);
    logV(fmt, argp);
    va_end(argp);
//    log_leave();
}


void log_init()
{
    MUTEX_INIT(&log_critical);
}


void log_free()
{
    MUTEX_FREE(&log_critical);
}


void log_enter()
{
    MUTEX_LOCK(&log_critical);
}


void log_leave()
{
    MUTEX_UNLOCK(&log_critical);
}
