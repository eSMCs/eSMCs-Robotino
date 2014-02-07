// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ACCELEROMETERVIEW_H
#define ACCELEROMETERVIEW_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class Robotino;

class AccelerometerView : public QwtPlot
{
    Q_OBJECT

public:
  AccelerometerView(Robotino *, QWidget *parent = 0);
  void newData();

private:
  QVector<double> x, y1, y2, y3;
  QwtPlotCurve c1, c2, c3;
  Robotino *robot;
};

#endif // ACCELEROMETERVIEW_H
