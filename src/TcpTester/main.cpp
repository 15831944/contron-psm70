#include "mainwindow.h"
#include <QApplication>

#include "sys/platform.h"
#include <QTextCodec>
#include <QDebug>
#include <QMetaMethod>
#include "log.h"
#include "tcp.h"

#if QT5
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Q_UNUSED(type)
    QString log = QString(msg.toLocal8Bit());
#else

void myMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type)
    QString log = QTextCodec::codecForName("GBK")->toUnicode(msg);
#endif
    if(NULL!=MainWindow::getSingletone())
    {
//        MainWindow::getSingletone()->log(log);
        QByteArray normalizedSignature = QMetaObject::normalizedSignature("writeLog(const QString &)");
        int methodIndex = MainWindow::staticMetaObject.indexOfMethod(normalizedSignature);
        QMetaMethod method = MainWindow::staticMetaObject.method(methodIndex);
        method.invoke(MainWindow::getSingletone(),
                      Qt::QueuedConnection,
                      Q_ARG(const QString&, log));
    }

//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s\n", msg);
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s\n", msg);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "Critical: %s\n", msg);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "Fatal: %s\n", msg);
//        abort();
//    }

}

int main(int argc, char *argv[])
{
	tcp_init();
#if QT5
    qInstallMessageHandler(myMessageOutput);
#else
    qInstallMsgHandler(myMessageOutput);
#endif

    QApplication a(argc, argv);

    MainWindow w;

    MainWindow::setSingletone(&w);

    w.show();

    APP_LOG("test for log");


    return a.exec();
}
