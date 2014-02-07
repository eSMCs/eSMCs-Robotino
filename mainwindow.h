// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "smcthread.h"

namespace Ui {
    class MainWindow;
}

class Robotino;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

public slots:
    void robotinoWindowDestroyed();

signals:
    void robotinoDestroyed();
    void robotinoCreated();

private:
    Ui::MainWindow *ui;
    Robotino *robotino;
    SMCThread smcThread;
    void readSettings();
    void writeSettings();

private slots:
    void on_actionRobotino_activated();
    void on_actionRobotino_Features_activated();
    void on_actionSMC_activated();
};

#endif // MAINWINDOW_H
