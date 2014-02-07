// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include <sys/timeb.h>
#include "robotinocamera.h"

using namespace rec::robotino::com;

RobotinoCamera::RobotinoCamera() : QObject(), Camera(),
cameraImage(), mutex()
{
}

void RobotinoCamera::imageReceivedEvent( const unsigned char* data,
                                        unsigned int dataSize,
                                        unsigned int width,
                                        unsigned int height,
                                        unsigned int numChannels,
                                        unsigned int bytesPerChannel,
                                        unsigned int step )
{
  mutex.lock();
  cameraImage = QImage( data, width, height, QImage::Format_RGB888 ).copy();
  mutex.unlock();
  emit newCameraImage();
}

void RobotinoCamera::getImage(QImage &img)
{
  mutex.lock();
  img = cameraImage.copy();
  mutex.unlock();
}
