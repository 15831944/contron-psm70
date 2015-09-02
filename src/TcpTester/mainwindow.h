#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tcpclient.h"
#include "tcpserver.h"
#include "itcpclient.h"
#include "itcpserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public BaseObject, public ITcpServer, public ITcpClient
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void setSingletone(MainWindow *win);
    static MainWindow *getSingletone();

signals:
    void log(const QString &text);

public:
    void addNewClient(char *ip, int port, SOCKET_HANDLE fd);
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientError(void *tcp);

public:
    bool isConnected();
    void heartbeat();
    void updateSendTime();
    bool isSendFail();
    void tryBreakConnection();
    bool isSendTime();
    void clearConnectCount();
    void clearHeartbeatCount();
    void enableHeartbeat();
    void disableHeartbeat();

protected slots:
    void writeLog(const QString &text);

private slots:
    void on_pushButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    TcpServer *mServer;
    TcpClient *mClient;

    //for heartbeat
    int mSendTime;
    int mHeartbeatCount;
    int mSendInterval;
    int mConnectCount;
    pthread_t mHeartbeatThread;
};

#endif // MAINWINDOW_H
