// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include <QtGui>
#include "robotino.h"
#include "audioview.h"


AudioView::AudioView(Robotino *rob, int nsamples, int nfreqbands, QWidget *parent) : QWidget(parent),
  imageLeft(QSize(nsamples, nfreqbands), QImage::Format_RGB888),
  imageRight(QSize(nsamples, nfreqbands), QImage::Format_RGB888),
  robot(rob), NSAMPLES(nsamples), NFREQBANDS(nfreqbands)
{
}

void AudioView::paintEvent( QPaintEvent*)
{
  if(isEnabled())
  {
    QPainter painter( this );
    painter.drawImage(QRect(rect().x(), rect().y(), rect().width(), rect().height()/2-1), imageLeft );
    painter.drawImage(QRect(rect().x(), rect().height()/2+1, rect().width(), rect().height()/2-1), imageRight );
  }
}


void AudioView::newData()
{
  QByteArray *rateMap = robot->getRateMap();
  if(rateMap[0].size() > 0)
  {
    for(int y=0; y<8; y++)
      for(int x=0;x<NSAMPLES; x++)
      {
        unsigned d = rateMap[0+y][rateMap[0+y].size()-NSAMPLES+x];
        imageLeft.setPixel(x, 8-y-1, d<<8|d);
        d = rateMap[8+y][rateMap[8+y].size()-NSAMPLES+x];
        imageRight.setPixel(x, 8-y-1, d<<8|d);
      }
    update();
  }
}

void AudioView::setData(QByteArray *data)
{
  for(int f=0; f<NFREQBANDS; f++)
    for(int i=0; i<qMin(NSAMPLES, data[f].size()); i++)
    {
      unsigned d = data[f][i];
      imageLeft.setPixel(i, NFREQBANDS-f-1, d<<8|d);
      d = data[NFREQBANDS+f][i];
      imageRight.setPixel(i, NFREQBANDS-f-1, d<<8|d);
    }
}
