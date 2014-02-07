// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include "actionobservationview.h"
#include <iostream>

ActionObservationView::ActionObservationView(QWidget *parent) :
    QWidget(parent),
    image(":/robotino-icon.png"),
    heading(9), value(0)
{
  brush.setColor(QColor(0,155,10));
  brush.setStyle(Qt::SolidPattern);
  pen.setColor(Qt::darkGreen);
  pen.setWidth(5);
  pen.setJoinStyle(Qt::RoundJoin);
  currentActionPen.setWidth(8);
  currentActionPen.setJoinStyle(Qt::RoundJoin);
  nextActionPen.setWidth(4);
  nextActionPen.setJoinStyle(Qt::RoundJoin);
  float angle = 360.0/9.0;
  for(unsigned i=0; i<9; i++)
  {
    QPointF direction(cos((360-i*angle-90.0)/360.0*2*M_PI),
                sin((360-i*angle-90.0)/360.0*2*M_PI) );
    heading[i] = direction;
    data[i] = 0;
  }
  for(int i=0; i<3; i++)
    currSpeed[i] = nextSpeed[i] = 0;
}

void ActionObservationView::newData(float speed[3], QByteArray action, QByteArray distance, float theValue)
{
  for(int i=0; i<9; i++)
  {
    data[i] = (distance[i] & 0x03) / 3.0;
  }

  for(int i=0; i<3; i++)
  {
    currSpeed[i] = speed[i];
    nextSpeed[i] = ((unsigned char)action[i]) - 5;
  }

  value = theValue;
  if(value<-1)
    value = -1;

  update();
}

void ActionObservationView::paintEvent(QPaintEvent*)
{
  QPainter painter( this );
  float scale = std::min(rect().width(),rect().height())*0.9;

  painter.setPen(Qt::NoPen);
  painter.setBrush(brush);
  QPointF beam[9];
  for(unsigned i=0; i<9; i++)
    beam[i] = heading[i] * scale/2 * data[i] + rect().center();
  painter.drawPolygon(beam, 9);

  // draw current motion
  QLineF move(rect().center(), QPointF(-currSpeed[1]*0.7*scale/2+rect().center().x(),
                                       -currSpeed[0]*0.7*scale/2+rect().center().y()));
  currentActionPen.setColor(QColor(250,250,250));
  painter.setPen(currentActionPen);
  painter.drawLine(move);

  // draw next action
  move = QLineF(rect().center(), QPointF(-nextSpeed[1]*0.7*scale/2+rect().center().x(),
                                         -nextSpeed[0]*0.7*scale/2+rect().center().y()));
  //value *= 3;
  if(value>1)
    value = 1;
  else if(value<-1)
    value = -1;
 if(value==0.0)
    nextActionPen.setColor(QColor(155,155,155));
  else if(value>0.0)
    nextActionPen.setColor(QColor(155+(int)(value*100),155,155));
  else if(value<0.0)
    nextActionPen.setColor(QColor((int)(-value*127)+127,(int)(value*127)+127, (int)(value*127)+127));
  painter.setPen(nextActionPen);
  painter.drawLine(move);
}
