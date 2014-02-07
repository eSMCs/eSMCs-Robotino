//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _CAMERAVIEW_H_
#define _CAMERAVIEW_H_

#include <QWidget>

class Robotino;

class CameraView : public QWidget
{
	Q_OBJECT
public:
  CameraView(Robotino *);
  void newData();

private:
  void paintEvent( QPaintEvent* e );
  QImage image;
  Robotino *robot;
};

#endif
