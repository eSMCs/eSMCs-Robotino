// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOCAMERA_H
#define ROBOTINOCAMERA_H

#include <QImage>
#include <QMutex>
#include <QObject>
#include "rec/robotino/com/Camera.h"

class RobotinoCamera : public QObject, public rec::robotino::com::Camera
{
  Q_OBJECT

public:
  RobotinoCamera();
  virtual void imageReceivedEvent( const unsigned char* data,
                                 unsigned int dataSize,
                                 unsigned int width,
                                 unsigned int height,
                                 unsigned int numChannels,
                                 unsigned int bytesPerChannel,
                                 unsigned int step );
  void getImage(QImage&);

signals:
  void newCameraImage();

private:
  QImage cameraImage;
  QMutex mutex;
};

#endif // ROBOTINOCAMERA_H
