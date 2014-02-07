// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include <Qt>
#include "robotinoagent.h"
#include "robotino.h"

RobotinoAgent::RobotinoAgent() : Agent(),
  workerThread(this)
{
}

void RobotinoAgent::startActing()
{
  if(!nextAction.isEmpty())
  {
    connect(robotino, SIGNAL(haveNewData(unsigned long)),
            SLOT(finishAction(unsigned long)));
    action = QByteArray(nextAction);
    executeAction();
  }
}

void RobotinoAgent::executeAction()
{
  if((unsigned char)action[0]<128)
  {
    // Set velocity
    robotino->setAcceleration(0, 0, 0);
    robotino->setVelocity(action[0]-50.0, action[1]-50.0, action[2]-50.0);
  }
  else
  {
    // Set acceleration
    robotino->setAcceleration(((unsigned char)action[0])-128-10,
                            ((unsigned char)action[1])-128-10,
                            ((unsigned char)action[2])-128-10);
  }
  robotino->playTone(action[3]);
  robotino->setLED(action[4]);
}

void RobotinoAgent::finishAction(unsigned long endTime)
{
  if(endTime - startTime != 10)
    std::cout << "Intervalle: " << endTime-startTime << std::endl;

  fetchSensorData();
  workerThread.start();
  if(!nextAction.isEmpty())
  {
    action = QByteArray(nextAction);
    nextAction.clear();
    executeAction();
  }
  else
  {
    std::cout << "Stop" << std::endl;
    disconnect(robotino, SIGNAL(haveNewData(unsigned long)),
               this, SLOT(finishAction(unsigned long)));
  }

}
void RobotinoAgent::processRobotinoFileData()
{
}


void RobotinoAgent::setRobot(Robot *robot)
{
  robotino = (Robotino *)robot;
  connect(robotino,SIGNAL(haveNewFileData()),SLOT(processRobotinoFileData()),
          Qt::UniqueConnection);
}

void RobotinoAgent::WorkerThread::run()
{
  agent->computeFeatures();
  agent->computeInternalValue();
  agent->emit newActionObservationPair();
}
