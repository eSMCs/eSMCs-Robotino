// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOFEATUREWINDOW_H
#define ROBOTINOFEATUREWINDOW_H

#include <QFrame>
#include <QTimer>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "audioview.h"

namespace Ui {
    class RobotinoFeatureWindow;
}

class SMCThread;
class ImageSeqView;

class RobotinoFeatureWindow : public QFrame
{
    Q_OBJECT

public:
  explicit RobotinoFeatureWindow(SMCThread *, QWidget *parent = 0);
  ~RobotinoFeatureWindow();
  void readSettings();

public slots:
    void newActionObservationPair();
    void updateRawViews();
    void updateFeatureViews();

private:
    void writeSettings();

    Ui::RobotinoFeatureWindow *ui;
    AudioView *audio;
    ImageSeqView *imageSeqView;
    SMCThread *smcThread;
    QVector<double> x, featX, /*featVoltage,*/ featAcceleration[3], featMotorCurrent[3];
    QVector<double> featDistance[9];
    QVector<double> internalValueData[5];

    QwtPlotCurve motorCurve[3], powerCurve[2], distanceCurve[9], accelCurve[3];
    QwtPlotCurve featDistanceCurve[9], featMotorCurve[3], /*featVoltageCurve,*/ featAccelCurve[3];
    QwtPlotCurve internalValueCurve[5];

protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // ROBOTINOFEATUREWINDOW_H
