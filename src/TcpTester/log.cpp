#include "log.h"

#include <stdarg.h>

#include <QDebug>
#include <QDateTime>

#define MAX_SIZE 10240

void _log(const char *fmt, ...)
{
    char buffer[MAX_SIZE];

    va_list argp;
    va_start(argp, fmt);
    memset(buffer, 0, MAX_SIZE);
    vsnprintf(buffer, MAX_SIZE, fmt, argp);
    va_end(argp);

    QString str = QString("%1%2%3")
            .arg(QDateTime::currentDateTime().toString("HH::mm:ss:zzz"))
            .arg("    ")
            .arg(QString(buffer));

    qDebug() << str;
}
