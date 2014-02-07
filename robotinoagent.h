// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOAGENT_H
#define ROBOTINOAGENT_H

#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QVector>
#include "agent.h"

class Robotino;

class RobotinoAgent : public Agent
{
  Q_OBJECT

public:
  RobotinoAgent();
  void setRobot(Robot *);
  virtual void startActing();

  QVector<double> rawDistance[9];
  QVector<double> rawMotorCurrent[3];
  QVector<double> rawAccelerations[3];
  QVector<double> rawPower[2];
  bool rawBumper;

public slots:
  void processRobotinoFileData();
  void finishAction(unsigned long);

private:
}

#endif // ROBOTINOAGENT_H
