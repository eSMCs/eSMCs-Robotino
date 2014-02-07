// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <QtGui>
#include "imageseqview.h"

ImageSeqView::ImageSeqView(QWidget *parent) :
    QWidget(parent)
{
}

void ImageSeqView::setData(QVector<QImage>& data)
{
  imageSeq = QVector<QImage>(data);
}

void ImageSeqView::paintEvent( QPaintEvent*)
{
  QRect r;
  if(imageSeq.size()>0)
  {
    QPainter painter( this );
    float scale = qMin(rect().width()/5.0/(double)imageSeq[0].width(),
                         rect().height()/2.0 / (double) imageSeq[0].height());
    for(int i=0; i<qMin(10, imageSeq.size()); i++)
    {
      QRect r((i%5)*imageSeq[0].width()*scale, (i/5)*imageSeq[0].height()*scale,
                imageSeq[i].width()*scale, imageSeq[i].height()*scale);
      painter.drawImage(r, imageSeq[i] );
    }
  }
}
