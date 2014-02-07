// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include <values.h>
#include "smcthread.h"
#include "robotino.h"

SMCThread::SMCThread(QObject *parent) : QThread(parent),
  robotino(NULL),
  mode(idle),
  serial(1),
  data()
{
  action = QByteArray(ACTIONSIZE,0);
  nextAction = QByteArray(ACTIONSIZE,0);
  observation = QByteArray(OBSERVATIONSIZE,0);
  internalValue = QVector<float>(5, 0.0);
  lastInternalValue = QVector<float>(4, 0.0);
  reliability = QVector<double>(NACTIONS,0);
  value = QVector<double>(NACTIONS, 0);
  predictionHist = QVector<QList<SMCPrediction > >(4);
  correctPredictionLength = QVector<double>();
  keytree[0] = &rootSMC.nextSMC;
  for(int i=1; i<HISTORYLENGTH+1; i++)
  {
    keytree[i] = NULL;
    currentSMC[i] = lastSMC[i] = NULL;
  }
  nSMCs = QVector<double>(HISTORYLENGTH+1, 0);
  for(int i=0; i<3; i++)
  {
    accel[i] = 0;
  }
  rootSMC.serial = 0;
  rootSMC.internalValue = rootSMC.externalValue = 0.0;
  rootSMC.count = 0;
}

int SMCThread::action2nr(QByteArray action)
{
  // 9 actions: forward, backward, left, right
//  return ((unsigned char)action[1]-5+1)*3
//        + (unsigned char)action[0]-5+1;


  // 6 actions: accelerate left/right/forward/backward, do nothing and stop
//  if((unsigned char)action[0]==5 && (unsigned char)action[1]==5 && (unsigned char)action[2]==5)
//    return 0; //stop
//  else if((unsigned char)action[0]==138 && (unsigned char)action[1]==138 && (unsigned char)action[2]==138)
//    return 1; //do nothing
//  else if((unsigned char)action[0]==137 && (unsigned char)action[1]==138 && (unsigned char)action[2]==138)
//    return 2; //do nothing
//  else if((unsigned char)action[0]==139 && (unsigned char)action[1]==138 && (unsigned char)action[2]==138)
//    return 3; //do nothing
//  else if((unsigned char)action[0]==138 && (unsigned char)action[1]==137 && (unsigned char)action[2]==138)
//    return 4; //do nothing
//  else if((unsigned char)action[0]==138 && (unsigned char)action[1]==139 && (unsigned char)action[2]==138)
//    return 5; //do nothing

//  std::cout << "!! Unkown action: " << (int)((unsigned char)action[0]) << " "
//                 << (int)((unsigned char)action[1]) << " " << (int)((unsigned char)action[2])<< std::endl;
//  return 6;

  //4 actions: accelerate forward/backward, do nothing and stop
//  if((unsigned char)action[0]==5)
//    return 0; //stop
//  else if((unsigned char)action[0]==138)
//    return 1; //do nothing
//  else if((unsigned char)action[0]==137)
//    return 2; //accel backward
//  else if((unsigned char)action[0]==139)
//    return 3; //accel forward

//  std::cout << "!! Unkown action: " << (int)((unsigned char)action[0]) << std::endl;
//  return 4;

// 4 actions: forward/backward/left/right
//  switch((unsigned char)action[0])
//  {
//    case 4: return 0; break;
//    case 6: return 1; break;
//    case 5: if((unsigned char)action[1]==4)
//        return 2;
//      else
//        return 3;
//      break;
//  }
//  std::cout << "!! Unkown action: " << (int)((unsigned char)action[0]) << " "
//               << (int)((unsigned char)action[1]) << std::endl;
//  return 4;

  // 2 actions: forward/backward
  if((unsigned char)action[0] == 4 || ((unsigned char)action[0]==5 && (unsigned char)action[1] == 5))
    return 0;
  else return 1;
}

QByteArray SMCThread::nr2action(int nr)
{
  QByteArray action(ACTIONSIZE,0);

  // 9 actions: forward, backward, left, right
//  action[0] = (nr%3-1) + 5;
//  action[1] = ((nr/3)%3-1) + 5;
//  action[2] = 5;

//  // 6 actions: accelerate left/right/forward/backward, do nothing and stop
//  switch(nr)
//  {
//    case 0: action[0] = action[1] = action[2] = 5; break; // stop
//    case 1: action[0] = action[1] = action[2] = 138; break; // do nothing
//    case 2: action[0] = 137; action[1] = action[2] = 138; break; //accel back
//    case 3: action[0] = 139; action[1] = action[2] = 138; break; // accel forw
//    case 4: action[0] = action[2] = 138; action[1] = 137; break; // accel right
//    case 5: action[0] = action[2] = 138; action[1] = 139; break; // accel left
//  }

  // 4 actions: accelerate forward/backward, do nothing and stop
//  switch(nr)
//  {
//    case 0: action[0] = action[1] = action[2] = 5; break; // stop
//    case 1: action[0] = action[1] = action[2] = 138; break; // do nothing
//    case 2: action[0] = 137; action[1] = action[2] = 138; break; //accel back
//    case 3: action[0] = 139; action[1] = action[2] = 138; break; // accel forw
//  }

// 4 actions: forward/backward/left/right
//  switch(nr)
//  {
//    case 0: action[0] = 4; action[1] = 5; break;
//    case 1: action[0] = 6; action[1] = 5; break;
//    case 2: action[0] = 5; action[1] = 4; break;
//    case 3: action[0] = 5; action[1] = 6; break;
//  }
//  action[2] = 5;

  // 2 actions: forward/backward
  if(nr==0)
    action[0] = 4;
  else
    action[0] = 6;
  action[1] = action[2] = 5;

  return action;
}

void SMCThread::newRawData()
{
  switch(mode)
  {
    case randomExploration:
      computeFeatures();
      emit newFeatures();
      chooseRandomAction();
      executeAction();
      break;

    case SMCControl:
      computeFeatures();
      emit newFeatures();
      computeInternalValue();
      updateSMCs();
      buildPredictions();
      chooseAction();
      executeAction();
      emit newSMC();
      break;

    case SMCControlSingleStep:
      executeAction();
      emit newSMC();
      computeFeatures();
      emit newFeatures();
      computeInternalValue();
      updateSMCs();
      break;
    default: ;
  }
}

void SMCThread::newRawFileData()
{
  computeFeatures();
  emit newFeatures();
  computeInternalValue();

  if(lastObservation.isEmpty())
  {
    lastObservation = observation;
    lastAction = action;
    lastInternalValue = internalValue;
  }
  else
  {
    QByteArray tmpObservation(lastObservation);
    lastObservation = observation;
    observation = tmpObservation;
    nextAction = action;
    action = lastAction;
    lastAction = nextAction;
    QVector<float> tmpInternalValue(lastInternalValue);
    lastInternalValue = internalValue;
    internalValue = tmpInternalValue;

    updateSMCs();
    buildPredictions();
    chooseAction();
    emit newSMC();

  }
}

void SMCThread::updateSMCs()
{
  emit message(Qt::gray, QString("********************"));

  currentValue = internalValue[0] + internalValue[1] + internalValue[2] + internalValue[3];
  emit message(Qt::yellow, QString("Current value: %1 action %2 (accel=%3 curr=%4 bmp=%5)")
               .arg(currentValue).arg(action2nr(action))
               .arg(internalValue[0]).arg(internalValue[1]).arg(internalValue[2]));
  internalValue[4] = currentValue;

  QByteArray key(action);
  key.append(observation);
  actionObservationHistory.prepend(key);
  for(int h=HISTORYLENGTH; h>=0; h--)
  {
    nSMCs[h] = 0;
    if(keytree[h])
    {
      currentSMC[h] = &((*keytree[h])[key]);
      currentSMC[h]->count++;
      currentSMC[h]->internalValue += currentValue;
      if(currentSMC[h]->count<=1)
      {
        currentSMC[h]->serial = serial++;
      }
      keytree[h+1] = &currentSMC[h]->nextSMC;
      // Check how many different actions have been observed from here
      QVector<int> haveAction(NACTIONS, 0);
      for(QHash<QByteArray, SMC>::const_iterator i=currentSMC[h]->nextSMC.constBegin();
          i!=currentSMC[h]->nextSMC.constEnd(); i++)
        haveAction[action2nr(i.key().left(ACTIONSIZE))] = 1;
      int sum = 0;
      for(int i=0; i<haveAction.size(); i++)
        sum += haveAction[i];
      nSMCs[h] = (double)sum/NACTIONS;
    }
  }

  // Evaluate predictions
  int hit = 0;
  for(int h=predictionHist.size()-1; h>=0; h--)
  {
    QList<SMCPrediction>::iterator i=predictionHist[h].begin();
    while(i!=predictionHist[h].end())
    {
      SMCPrediction p = *i;
      if(p.actionObservationSequence.left(ACTIONSIZE+OBSERVATIONSIZE)!=key)
        i = predictionHist[h].erase(i);
      else
      {
        p.actionObservationSequence.remove(0, ACTIONSIZE+OBSERVATIONSIZE);
        i++;
      }
    }
    if(!predictionHist[h].isEmpty() && h+1>hit)
      hit = h+1;
    if(h>0)
      predictionHist[h] = predictionHist[h-1];
  }
  correctPredictionLength.append(hit);
}

void SMCThread::buildPredictions()
{
  prediction.clear();
  havePredictionForAction = QVector<bool>(NACTIONS,false);
  // Find the longest SMC that has the current action in its nextSMC map
  for(int h=HISTORYLENGTH-1; h>=0; h--)
  {
    if(currentSMC[h]==NULL || currentSMC[h]->nextSMC.isEmpty())
    {
      continue;
    }
    else
    {
      for(QHash<QByteArray, SMC>::const_iterator currAct=currentSMC[h]->nextSMC.constBegin();
          currAct!=currentSMC[h]->nextSMC.constEnd(); currAct++)
      {
        // Action sequence must start with currently executed action (nextAction has the efference copy)
        if(currAct.key().left(ACTIONSIZE)==nextAction)
        {
          QVector<SMCPredictionStep> simActSeq;
          SMCPredictionStep step1;
          step1.action = action2nr(currAct.key().left(ACTIONSIZE));
          step1.value = currAct.value().internalValue;
          step1.count = currAct.value().count;
          step1.reliability = h+1;
          simActSeq.append(step1);
          QByteArray simActObsHist(actionObservationHistory);
          simActObsHist.prepend(currAct.key());
          traverseSMCTree(currAct.value().nextSMC, simActObsHist, simActSeq,
                          h+1, 1);
        }
      }
    }
    // If we have predictions for all actions, don't look at predictions from
    // shorter history lengths
  }
  predictionHist[0] = prediction;
}

double SMCThread::traverseSMCTree(QHash<QByteArray, SMC> nextSMC,
                                  QByteArray actObsHist, QVector<SMCPredictionStep> smcSeq,
                                  int reliability, int depth)
{
  for(QHash<QByteArray, SMC>::const_iterator i=nextSMC.constBegin(); i!=nextSMC.constEnd(); i++)
  {
    SMC smc = i.value();

    if(depth<PREDICTIONDEPTH+1)
    {
      if(!smc.nextSMC.isEmpty())
      {
        // Continue traversing based on previously observed SMCs as recorded in nextSMC
        QByteArray aoh(actObsHist);
        aoh.prepend(i.key());
        QVector<SMCPredictionStep> tmpSMCSeq(smcSeq);
        SMCPredictionStep step;
        step.action = action2nr(i.key().left(ACTIONSIZE));
        step.value = smc.internalValue;
        step.count = smc.count;
        step.reliability = reliability+1;
        tmpSMCSeq.append(step);
        traverseSMCTree(smc.nextSMC, aoh, tmpSMCSeq, reliability+1, depth+1);
      }
      else
      {
      }
    }
    else
    {
      SMCPrediction p;
      p.smcSequence = smcSeq;
      p.actionObservationSequence = actObsHist.left((depth+2)*(ACTIONSIZE+OBSERVATIONSIZE));
      prediction.append(p);
    }
  }
  return 0.0;
}

void SMCThread::chooseAction()
{
  // For each action, find the maximum reliability (longst SMCs used for predicting)
  reliability = QVector<double>(NACTIONS,0);
  count = QVector<int>(NACTIONS, 0);
  value = QVector<double>(NACTIONS, 0);

  // For now we are only interested in the most reliable predictions
  // So generate a sub-list with those
  QVector<QList<SMCPrediction> > mostReliablePrediction(NACTIONS);
  for(QList<SMCPrediction>::const_iterator i=prediction.constBegin();
      i!=prediction.constEnd(); i++)
  {
    SMCPrediction p = *i;
    if(p.smcSequence.size()<2)
      continue;
    int action = p.smcSequence[1].action;
    if(p.smcSequence[1].reliability > reliability[action])
    {
      reliability[action] = p.smcSequence[1].reliability;
      mostReliablePrediction[action] = QList<SMCPrediction>();
      mostReliablePrediction[action].append(p);
    }
    else if(p.smcSequence[1].reliability == reliability[action])
      mostReliablePrediction[action].append(p);
  }

  for(int action=0; action<NACTIONS; action++)
    if(!mostReliablePrediction[action].isEmpty())
    {
      emit message(Qt::gray, QString("Predictions for action %1, reliability %2")
                   .arg(action).arg(reliability[action]));

      int j=0;
      for(QList<SMCPrediction>::const_iterator i=mostReliablePrediction[action].constBegin();
          i!=mostReliablePrediction[action].constEnd(); i++, j++)
      {
        SMCPrediction p = *i;
        value[action] += p.smcSequence[0].count * p.smcSequence[1].value;
        count[action] += p.smcSequence[0].count * p.smcSequence[1].count;

        QString str = QString("%1: ").arg(j);
        for(int t=0; t<p.smcSequence.size(); t++)
        {
          SMCPredictionStep s = p.smcSequence[t];
          str += QByteArray::number(s.action) + " ("
            + QByteArray::number(s.value/s.count) + ":"
            + QByteArray::number((s.count)) + ":"
            + QByteArray::number(s.reliability) + ") ";
        }
        str += " -> " + QByteArray::number(value[action]) + " (n="
            + QByteArray::number(count[action]) + " rel="
            + QByteArray::number(reliability[action]) + ")";
        emit message(Qt::gray, str);

      }
    }
    else
    {
      value[action] = drand48() - 1;
      count[action] = 1;
    }

  double bestValue = value[0]/count[0];
  int minReliability = reliability[0];
  int minReliableMinCount = count[0];
  int bestAction = 0;
  for(int i=1; i<NACTIONS; i++)
  {
    if(value[i]/count[i]>bestValue)
      bestValue = value[i]/count[i];
    if(reliability[i]<minReliability)
    {
      minReliability = reliability[i];
      minReliableMinCount = count[i];
    }
    else if(reliability[i]==minReliability && count[i]<minReliableMinCount)
      minReliableMinCount = count[i];
  }

  for(int i=0; i<NACTIONS; i++)
  {
    emit message(Qt::gray, QString("action %1: value=%2 count=%3 rel=%4")
                 .arg(i).arg(value[i]/count[i]).arg(count[i]).arg(reliability[i]));
  }
  emit message(Qt::gray, QString("bestValue=%1 minReliability=%2 minReliableMinCount=%3")
               .arg(bestValue).arg(minReliability).arg(minReliableMinCount));
  bool LED;
  double r = drand48();
  QVector<int> bestActions;
  if(r<bestValue+1)
  {
    // Choose (from best) predicted action(s)
    for(int i=0; i<NACTIONS; i++)
      if(value[i]/count[i]==bestValue)
        bestActions.append(i);
    bestAction = bestActions[(int)(drand48()*bestActions.size()-1e-6)];
    emit message(Qt::green,QString("Choose from %1 best prediction(s): %2")
                 .arg(bestActions.size()).arg(bestAction));
    anticipatedValue = bestValue;
    LED = false;
    if(bestAction<0)
      std::cout << "Mist1"<< std::endl;
  }
  else
  {
    {
      // Try action with least reliability
      for(int i=0; i<NACTIONS; i++)
        if(reliability[i]==minReliability && count[i]==minReliableMinCount)
          bestActions.append(i);
      bestAction = bestActions[(int)(drand48()*bestActions.size()-1e-6)];
      emit message(Qt::red, QString("Choose least reliable (rel=%1 cnt=%2) action: %3")
                   .arg(minReliability).arg(minReliableMinCount).arg(bestAction));
      LED = true;
      anticipatedValue = 0;
    }
    if(bestAction<0)
      std::cout << "Mist"<< std::endl;
  }
  robotino->setLED(LED);

  nextAction = nr2action(bestAction);
  nextAction[3] = 0;   // sound
  nextAction[4] = 0;  // LED
  emit newAction();

}

double SMCThread::evaluateActionSequence(QByteArray actObsHist, QVector<SMCPredictionStep> smcSeq,
                                         int depth)
{
  int h;

  // Find longest SMC history matching actObsHist
  QHash<QByteArray, SMC>::iterator i
      = rootSMC.nextSMC.find(actObsHist.left(ACTIONSIZE+OBSERVATIONSIZE));
  if(i!=rootSMC.nextSMC.end())
  {
    SMC smc = i.value();
    for(h=1; h<HISTORYLENGTH &&
        (i=smc.nextSMC.find(actObsHist.mid(h*(ACTIONSIZE+OBSERVATIONSIZE),
        ACTIONSIZE+OBSERVATIONSIZE)))!=smc.nextSMC.end(); h++)
      smc = i.value();

    if(depth<PREDICTIONDEPTH+1 && !smc.nextSMC.isEmpty())
    {
      // Now try all sequences involving this SMC
      for(i=smc.nextSMC.begin(); i!=smc.nextSMC.end(); i++)
      {
        QByteArray aoh(actObsHist);
        aoh.prepend(i.key());
        QVector<SMCPredictionStep> tmpSMCSeq(smcSeq);
        SMCPredictionStep step;
        step.action = action2nr(i.key().left(ACTIONSIZE));
        step.value = i.value().internalValue;
        step.count = i.value().count;
        step.reliability = h+1;
        tmpSMCSeq.append(step);
        evaluateActionSequence(aoh, tmpSMCSeq, depth+1);
      }
    }
    else
    {
      SMCPrediction p;
      p.smcSequence = smcSeq;
      p.actionObservationSequence = actObsHist.left((depth+2)*(ACTIONSIZE+OBSERVATIONSIZE));
      prediction.append(p);
    }
  }
  else
    std::cout << "Nicht gefunden" << std::endl;
  return 0.0;
}

void SMCThread::executeAction()
{
  Action robotinoAction;
  for(int i=0; i<ACTIONSIZE; i++)
    robotinoAction.a[i] = (unsigned char)nextAction[i];
  robotino->nextAction.append(robotinoAction);
}

void SMCThread::chooseRandomAction()
{
  int act[3];

  nextAction[0] = 8 + act[0] + 128; // ax
  nextAction[1] = 8 + act[1] + 128; // ay
  nextAction[2] = 8 + act[2] + 128; // aomega
  nextAction[3] = 0;   // sound
  nextAction[4] = 0;  // LED
}

void SMCThread::stopAction()
{
  robotino->nextAction.clear();
  Action robotinoAction;
  for(int i=0; i<ACTIONSIZE; i++)
    robotinoAction.a[i] = 5;
  robotino->nextAction.append(robotinoAction);
}

void SMCThread::computeFeatures()
{
  // Distance feature
  for(unsigned i=0; i<9; i++)
  {
    // Compute avg distance in first, middle, and last part (overlapping windows)
    float avg = 0;
    for(int j=0; j<robotino->epochDistance[i].size(); j++)
    {
      avg += robotino->epochDistance[i][j];
    }

    avg /= robotino->epochDistance[i].size();
    int v = (int)(avg*2.9);
    if(v<0)
      v = 0;
    else if(v>2)
      v = 2;
      observation[7+i] = 0;
  }

  ///////////////////////////////
  // Acceleration features
  ////////////////////////////////
  for(unsigned i=0; i<2; i++)
  {
    // Compute base line, which yields avg acceleration in the range 0...15
    // stored in the lowest 4 bit
    QVector<double> d(robotino->epochAcceleration[i]);
    double mean = 0;
    for(int s=0; s<d.size(); s++)
      mean += d[s];
    mean /= d.size();

    double minmax = 0;
    for(int s=0; s<d.size(); s++)
      if(fabs(d[s]-mean)>fabs(minmax))
        minmax = d[s]-mean;

    mean = mean-0.5;
    if(mean<0.0)
      mean = 0;
    else if(mean>1.0)
      mean = 1;
    int meanFeature = (int)((mean-1e-6)*4.0);

    minmax *= 3.5;
    int minmaxFeature = 0;
    if(minmax<0)
    {
      minmaxFeature = (int)(minmax-0.5);
      if(minmaxFeature<-2)
      {
        std::cout << "minhit" << std::endl;
        minmaxFeature = -2;
      }
    }
    else
    {
      minmaxFeature = (int)(minmax+0.5);
      if(minmaxFeature>2)
      {
        minmaxFeature = 2;
        std::cout << "maxhit" << std::endl;
      }
    }
    observation[4+i] = (unsigned char)(5*meanFeature + minmaxFeature);
  }
  observation[6] = 10;

  //////////////////////////////////////
  // Motor current features
  //////////////////////////////////////

  float sum = 0;
  for(unsigned i=0; i<3; i++)
  {
    for(int j=0; j<robotino->epochMotor[i].size(); j++)
    {
      sum += robotino->epochMotor[i][j];
    }
  }
  sum /= robotino->epochMotor[0].size()*3;
  int meanFeature = (int)((sum-1e-6)*3.0); // for 1s periods
  if(meanFeature>2)
    meanFeature = 2;
  observation[1] = (unsigned)meanFeature;
  observation[2] = 0;//(unsigned)maxFeature;
  observation[3] = 0;

// ATTENTION: leaving the wall when the bumper has triggered,
// has active bumper values in the beginning of this (leaving) epoch. Therefore
// consider bumper value only from the end of an epoch
  bumper = robotino->epochBumper.last();
  observation[0] = bumper;

  for(int i=0; i<ACTIONSIZE; i++)
  {
    action[i] = (unsigned char)(robotino->action.a[i]);
  }
}

void SMCThread::computeInternalValue()
{
  int max = 0; // mask std
  for(unsigned i=0; i<3; i++)
  {
    int diff = observation[4+i]%5;
    if(diff>2)
      diff = 5-diff;
    if(diff > max)
      max = diff;
  }
  internalValue[0] = -0.2*max;

  internalValue[1] = -0.5*observation[1];

  // Bumper
  if(bumper)
  {
    internalValue[2] = -1;
  }
  else
    internalValue[2] = 0.0;

  internalValue[3] = 0;
}

void SMCThread::setRobotino(Robotino *r)
{
  robotino = r;
  if(robotino)
  {
    connect(robotino, SIGNAL(haveNewData(ulong)), SLOT(newRawData()));
    connect(robotino, SIGNAL(haveNewFileData()), SLOT(newRawFileData()));
  }
}

Robotino *SMCThread::getRobotino()
{
  return robotino;
}

void SMCThread::startFeatureComputation(bool start)
{
  stopAction();
  if(start)
    mode = randomExploration;
  else
    mode = idle;
}

void SMCThread::startSMCComputation(bool start)
{
  if(start)
  {
    for(int h=0; h<HISTORYLENGTH; h++)
      currentSMC[h] = lastSMC[h] = NULL;
    lastObservation.clear();

    Action robotinoAction;
    QByteArray randomAction = nr2action(0);
    for(int i=0; i<randomAction.size(); i++)
      robotinoAction.a[i] = (unsigned char)randomAction[i];
    robotinoAction.a[3] = 0; robotinoAction.a[4] = 0;
    robotino->nextAction.append(robotinoAction);
    nextAction = randomAction;
    nextAction[3] = 0; nextAction[4] = 0;
    mode = SMCControl;
  }
  else
  {
    Action robotinoAction;
    robotinoAction.a[0] = robotinoAction.a[1] = robotinoAction.a[2] = 5;
    robotino->nextAction.append(robotinoAction);
    mode = idle;
  }
}

void SMCThread::clearSMCs()
{
  for(int i=1; i<HISTORYLENGTH+1; i++)
  {
    keytree[i] = NULL;
    currentSMC[i] = lastSMC[i] = NULL;
  }
  for(int i=0; i<3; i++)
    accel[i] = 0;
  rootSMC.serial = 0;
  rootSMC.internalValue = rootSMC.externalValue = 0.0;
  rootSMC.count = 0;
  rootSMC.nextSMC.clear();
  serial = 1;
  emit message(Qt::red, QString("All SMCs cleared."));
}

void SMCThread::loadSMCs(QString filename)
{
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    QErrorMessage errmsg;
    errmsg.showMessage("Error opening " + filename);
    return;
  }
  data.setDevice(&file);

  treeDepth = 0;
  for(int h=0; h<HISTORYLENGTH; h++)
    nSMCs[h] = 0;

  loadNode(rootSMC);

  file.close();

  for(int h=0; h<HISTORYLENGTH; h++)
    emit message(Qt::white, QString("Loaded %1 SMCs of history length %2.").
                 arg(nSMCs[h]).arg(h+1));
}

void SMCThread::loadNode(SMC& smc)
{
  data >> smc.count >> smc.serial >> smc.internalValue >> smc.externalValue;
  if(smc.serial>serial)
    serial = smc.serial;

  smc.nextSMC.clear();
  char charVal;
  int nextSMCSize, keySize;
  data >> nextSMCSize;
  if(nextSMCSize>0)
  {
    nSMCs[treeDepth] += nextSMCSize;
    treeDepth++;
    for(int i=0; i<nextSMCSize; i++)
    {
      data >> keySize;
      QByteArray key(keySize,0);
      for(int k=0; k<keySize; k++)
      {
        data >> (quint8&)charVal;
        key[k] = charVal;
      }

      loadNode(smc.nextSMC[key]);
    }
    treeDepth--;
  }
}

void SMCThread::saveSMCs(QString filename)
{
  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly))
  {
    QErrorMessage errmsg;
    errmsg.showMessage("Error opening " + filename);
    return;
  }
  data.setDevice(&file);

  treeDepth = 0;
  for(int h=0; h<HISTORYLENGTH; h++)
    nSMCs[h] = 0;

  saveNode(rootSMC);

  file.close();
  for(int h=0; h<HISTORYLENGTH; h++)
    emit message(Qt::white, QString("Saved %1 SMCs of history length %2.").
                 arg(nSMCs[h]).arg(h+1));
}

void SMCThread::saveNode(const SMC& smc)
{
  data << smc.count << smc.serial << smc.internalValue << smc.externalValue;
  data << smc.nextSMC.size();
  if(smc.nextSMC.size()>0)
  {
    nSMCs[treeDepth] += smc.nextSMC.size();
    treeDepth++;
    int n=0;
    for(QHash<QByteArray, SMC>::const_iterator k = smc.nextSMC.constBegin();
        k != smc.nextSMC.constEnd(); k++, n++)
    {
      QByteArray key = k.key();
      data << key.size();
      for(int l=0; l<key.size(); l++)
      {
        data << (quint8)key[l];
      }
      saveNode(k.value());
    }
    treeDepth--;
  }
}
