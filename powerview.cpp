// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include <QtGui>
#include "powerview.h"
#include "robotino.h"

const unsigned NSamples = 100;

PowerView::PowerView(Robotino *rob, QwtPlot *amotorPlot, QwtPlot *apowerPlot) :
    QObject(),
    robot(rob),
    x(NSamples),
    I1(NSamples), I2(NSamples), I3(NSamples), I(NSamples), U(NSamples),
    cI1(), cI2(), cI3(), cI(), cU(),
    motorPlot(amotorPlot),
    powerPlot(apowerPlot)
{
  for(int i=0; i<x.size();i++)
    x[i] = i;
  cI1.setSamples(x, I1);
  cI1.setPen(QPen(QColor(200,80,40)));
  cI1.attach(motorPlot);
  cI2.setSamples(x, I2);
  cI2.setPen(QPen(QColor(80,200,40)));
  cI2.attach(motorPlot);
  cI3.setSamples(x, I3);
  cI3.setPen(QPen(QColor(80,40,200)));
  cI3.attach(motorPlot);
  cI.setSamples(x, I);
  cI.setPen(QPen(QColor(100,100,100)));
  cI.attach(powerPlot);
  cU.setSamples(x, U);
  cU.setPen(QPen(QColor(200,200,200)));
  cU.setYAxis(QwtPlot::yRight);
  cU.attach(powerPlot);
  powerPlot->enableAxis(QwtPlot::yRight);
  powerPlot->setAxisScale(QwtPlot::yLeft, 0, 0.5, 0.1);
  powerPlot->setAxisScale(QwtPlot::yRight, 20, 30, 2);
  motorPlot->setAxisScale(QwtPlot::yLeft, 0, 1.5, 0.1);
  motorPlot->setAxisMaxMinor(QwtPlot::yLeft, 0);
}

void PowerView::newData()
{
  QVector<double> *motorCurrent = robot->getMotorCurrents();
  if(motorCurrent)
  {
    for(unsigned i=0; i<NSamples; i++)
    {
      x[i] = i;
      I1[i] = motorCurrent[0][motorCurrent[0].size()-NSamples+i];
      I2[i] = motorCurrent[1][motorCurrent[1].size()-NSamples+i];
      I3[i] = motorCurrent[2][motorCurrent[2].size()-NSamples+i];
    }
    cI1.setSamples(x, I1);
    cI2.setSamples(x, I2);
    cI3.setSamples(x, I3);
    motorPlot->replot();
  }

  QVector<double> *UI = robot->getPower();
  if(UI)
  {
    for(unsigned i=0; i<NSamples; i++)
    {
      x[i] = i;
      U[i] = UI[0][UI[0].size()-NSamples+i];
      I[i] = UI[1][UI[1].size()-NSamples+i];
    }
    cU.setSamples(x, U);
    cI.setSamples(x, I);
    powerPlot->replot();
  }
}
