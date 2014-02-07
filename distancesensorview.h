// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef DISTANCESENSORVIEW_H
#define DISTANCESENSORVIEW_H

#include <QtGui>

class Robotino;

class DistanceSensorView : public QWidget
{
    Q_OBJECT

public:
    DistanceSensorView(Robotino *, unsigned ndir, QWidget *parent = 0);
    void newData();

  private:
    void paintEvent( QPaintEvent* e );
    QBrush brush;
    QPen pen;
    QImage image;
    QVector<QPointF> heading;
    QVector<double> *data;
    Robotino *robot;
    unsigned Ndirections;
  };

#endif // DISTANCESENSORVIEW_H
