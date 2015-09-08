#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tcpclient.h"
#include "tcpserver.h"
#include "itcpclient.h"
#include "itcpserver.h"

#include "remotepc.h"
#include "localpc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public BaseObject/*, public ITcpServer, public ITcpClient*/
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
    void addNewClient(void *tcp);
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientError(void *tcp);


protected slots:
    void writeLog(const QString &text);

private slots:
    void on_pushButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;

    LocalPC *mServer;

    RemotePC *mClient;

};

#endif // MAINWINDOW_H
