// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include "accelerometerview.h"
#include "robotino.h"

const unsigned NSamples = 100;

AccelerometerView::AccelerometerView(Robotino *rob, QWidget *parent) :
    QwtPlot(parent),
    x(NSamples),
    y1(NSamples), y2(NSamples), y3(NSamples),
    c1(), c2(), c3(),
    robot(rob)
{
  c1.setPen(QPen(QColor(200,200,0)));
  c1.attach(this);
  c2.setPen(QPen(QColor(0,200,0)));
  c2.attach(this);
  c3.setPen(QPen(QColor(0,0,200)));
  c3.attach(this);
  enableAxis(QwtPlot::yLeft, false);
  enableAxis(QwtPlot::xBottom, false);
}

void AccelerometerView::newData()
{
  QVector<double> *data = robot->getAccelerations();
  for(unsigned i=0;i<NSamples;i++)
  {
    x[i] = i;
    y1[i] = data[0][data[0].size()-NSamples+i];
    y2[i] = data[1][data[1].size()-NSamples+i];
    y3[i] = data[2][data[2].size()-NSamples+i];
  }
  c1.setSamples(x, y1);
  c2.setSamples(x, y2);
  c3.setSamples(x, y3);
  replot();
}
