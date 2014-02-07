// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <algorithm>
#include <iostream>
#include <math.h>
#include "robotino.h"
#include "distancesensorview.h"

DistanceSensorView::DistanceSensorView(Robotino *rob, unsigned ndir, QWidget *parent) :
    QWidget(parent),
    image(":/robotino-icon.png"),
    heading(ndir),
    data(NULL),
    robot(rob),
    Ndirections(ndir)
{
  brush.setColor(QColor(0,155,10));
  brush.setStyle(Qt::SolidPattern);
  pen.setColor(QColor(20,255,40));
  pen.setWidth(5);
  pen.setJoinStyle(Qt::RoundJoin);
  float angle = 360.0/ndir;
  for(unsigned i=0; i<ndir; i++)
  {
    QPointF direction(cos((360-i*angle-90.0)/360.0*2*M_PI),
                sin((360-i*angle-90.0)/360.0*2*M_PI) );
    heading[i] = direction;
  }
}

void DistanceSensorView::newData()
{
  data = robot->getDistances();
  if(data)
    update();
}

void DistanceSensorView::paintEvent(QPaintEvent*)
{
  if(!data)
    return;

  QPainter painter( this );
  float scale = std::min(rect().width(),rect().height());

  painter.setPen(Qt::NoPen);
  painter.setBrush(brush);
  QPointF beam[Ndirections];
  for(unsigned i=0; i<Ndirections; i++)
  {
    beam[i] = heading[i] * scale/2 * data[i].last() + rect().center();
  }
  painter.drawPolygon(beam, Ndirections);
  painter.setPen(pen);
  for(unsigned i=0; i<Ndirections; i++)
    painter.drawLine(rect().center(), beam[i]);

  QRect r(0, 0, 0.4*scale, 0.4*scale);
  r.moveTo((rect().width()-r.width())/2, (rect().height() - r.height())/2);

  painter.drawImage(r, image );
}
