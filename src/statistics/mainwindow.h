#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void analytic(const QString &fileName);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    long mMax;
    int mTestCount;
};

#endif // MAINWINDOW_H
