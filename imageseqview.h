// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef IMAGESEQVIEW_H
#define IMAGESEQVIEW_H

#include <QWidget>

class ImageSeqView : public QWidget
{
    Q_OBJECT
public:
  explicit ImageSeqView(QWidget *parent = 0);
  virtual void paintEvent( QPaintEvent*);
  void setData(QVector<QImage>&);

signals:

public slots:

protected:
  QVector<QImage> imageSeq;
};

#endif // IMAGESEQVIEW_H
