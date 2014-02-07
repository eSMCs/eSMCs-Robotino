// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef SMCWINDOW_H
#define SMCWINDOW_H

#include <QFrame>
#include <QHash>
#include <QQueue>
#include <qwt_plot_curve.h>
#include <qwt_plot_histogram.h>

class SMCThread;
class ActionObservationView;
class ActionValueView;

namespace Ui {
    class SMCWindow;
}

class SMCWindow : public QFrame
{
    Q_OBJECT

public:
  explicit SMCWindow(SMCThread *, QWidget *parent = 0);
  ~SMCWindow();
  void readSettings();

private slots:
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_fileChooseButton_clicked();
    void showMessage(QColor, QString);
    void updateActionValueDisplay();
    void updateVisualization();

private:
    void writeSettings();

    Ui::SMCWindow *ui;
    QByteArray fileDialogSaveState;
    SMCThread *smcThread;
    ActionObservationView *aoView;
    ActionValueView *avView;
    QwtPlotHistogram reliabilityHistogram;
    QwtPlotCurve predictionCurve;
    QVector<double> x;

protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // SMCWINDOW_H
