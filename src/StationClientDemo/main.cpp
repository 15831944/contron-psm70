#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString pluginPath = QApplication::applicationDirPath() + "/plugins";
    QApplication::addLibraryPath(pluginPath);

    MainWindow w;
    w.show();

    return a.exec();
}
