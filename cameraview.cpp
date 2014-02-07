//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include <iostream>
#include <QtGui>
#include "cameraview.h"
#include "robotino.h"

CameraView::CameraView(Robotino *rob) : QWidget(),
  image(),
  robot(rob)
{
}

void CameraView::paintEvent( QPaintEvent*)
{
  {
    QPainter painter( this );
    float scale = std::min(rect().width()/(float) image.width(),
                         rect().height() / (float) image.height());
    QRect r(0, 0, image.width()*scale, image.height()*scale);
    r.moveTo((rect().width()-r.width())/2, (rect().height() - r.height())/2);
    painter.drawImage(r, image );
  }
}


void CameraView::newData()
{
  robot->getImage(image);
  update();
}
