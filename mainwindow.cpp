// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014
 #include <iostream>
#include <QMdiSubWindow>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rec/robotino/com/all.h"
#include "robotinowindow.h"
#include "robotinofeaturewindow.h"
#include "smcwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    robotino(NULL),
    smcThread(this)
{
  ui->setupUi(this);
  connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
  smcThread.start();
  smcThread.setPriority(QThread::HighestPriority);
  readSettings();
  on_actionRobotino_activated();
  on_actionRobotino_Features_activated();
  on_actionSMC_activated();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  ui->mdiArea->closeAllSubWindows();
  smcThread.quit();
  smcThread.wait(5000);
  writeSettings();
  event->accept();
}

void MainWindow::readSettings()
{
  QSettings settings("UKE", "robotino1");
  restoreGeometry(settings.value("Geometry").toByteArray());
  restoreState(settings.value("State").toByteArray());
}

void MainWindow::writeSettings()
{
  QSettings settings("UKE", "robotino1");
  settings.setValue("Geometry", saveGeometry());
  settings.setValue("State", saveState());
}

void MainWindow::on_actionRobotino_activated()
{
  RobotinoWindow *win = new RobotinoWindow;
  QMdiSubWindow *subwin = ui->mdiArea->addSubWindow(win);
  if(subwin)
  {
    win->readSettings();
    subwin->show();
  }
  connect(subwin, SIGNAL(destroyed()), SLOT(robotinoWindowDestroyed()));
  robotino = win->getRobotino();
  smcThread.setRobotino(robotino);
  //emit robotinoCreated();
  ui->actionRobotino_Features->setEnabled(true);
}

void MainWindow::robotinoWindowDestroyed()
{
  robotino = NULL;
  smcThread.setRobotino(robotino);
  emit robotinoDestroyed();
  ui->actionRobotino_Features->setDisabled(true);
}

void MainWindow::on_actionRobotino_Features_activated()
{
  RobotinoFeatureWindow *win = new RobotinoFeatureWindow(&smcThread, this);
  QMdiSubWindow *subwin = ui->mdiArea->addSubWindow(win);
  if(subwin)
  {
    connect(this, SIGNAL(robotinoDestroyed()), subwin, SLOT(close()));
    win->readSettings();
    subwin->show();
  }
}

void MainWindow::on_actionSMC_activated()
{
  SMCWindow *win = new SMCWindow(&smcThread, this);
  QMdiSubWindow *subwin = ui->mdiArea->addSubWindow(win);
  if(subwin)
  {
    win->readSettings();
    subwin->show();
  }
}
