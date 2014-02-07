// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef POWERVIEW_H
#define POWERVIEW_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class Robotino;

class PowerView : public QObject
{
    Q_OBJECT
public:
  PowerView(Robotino *, QwtPlot *amotorPlot, QwtPlot *apowerPlot);
  void newData();

protected:
  Robotino *robot;
  QVector<double> x, I1, I2, I3, I, U;
  QwtPlotCurve cI1, cI2, cI3, cI, cU;
  QwtPlot *motorPlot, *powerPlot;
};

#endif // POWERVIEW_H
