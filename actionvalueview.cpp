// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include "actionvalueview.h"

ActionValueView::ActionValueView(QWidget *parent) :
  QWidget(parent),
  image(":/robotino-icon.png")
{
  actionPen.setJoinStyle(Qt::RoundJoin);
}

void ActionValueView::newData(QVector<double> value, QVector<double> reliability)
{
  viewData.clear();
  for(int actionnr=0; actionnr<value.size(); actionnr++)
  {
    if(reliability[actionnr]==0)
      continue;
    ActionViewData d;

    // 2 actions: forward/backward
    d.movementdir = QPointF(actionnr==0?-1:1, 0);
    d.rotation = 0;
    float v = value[actionnr];
    if(v<-1.0)
      v = -1.0;
    if(v<=0)
      d.color = QColor(-v*127+127, 127+v*127, 127+v*127);
    else
      d.color = QColor(0, 100, 200);
    d.width = 20*reliability[actionnr];
    viewData.append(d);
  }
  update();
}

void ActionValueView::paintEvent(QPaintEvent*)
{
  QPainter painter( this );

  float scale = std::min(rect().width(),rect().height())*0.9;
  for(int i=0; i<viewData.size(); i++)
  {
    QLineF move(rect().center(), QPointF(-viewData[i].movementdir.y()*0.7*scale/2,
                                         -viewData[i].movementdir.x()*0.7*scale/2)+rect().center());
    actionPen.setColor(viewData[i].color);
    actionPen.setWidth(viewData[i].width);
    painter.setPen(actionPen);
    painter.drawLine(move);
    QLineF omegaLine = move;
    if(omegaLine.length()>0)
    {
      omegaLine.setLength(scale/6);
      omegaLine.translate(move.p2()-move.p1());
    }
    else
      omegaLine = QLineF(rect().center(), rect().center()-QPointF(0,scale/6));
    omegaLine.setAngle(omegaLine.angle() + viewData[i].rotation*20);
    painter.drawLine(omegaLine);
  }

  // paint robot icon
  QRect r(0, 0, 0.3*scale, 0.3*scale);
  r.moveTo((rect().width()-r.width())/2, (rect().height() - r.height())/2);
}
