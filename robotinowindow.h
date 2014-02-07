// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOWINDOW_H
#define ROBOTINOWINDOW_H

#include <QFrame>
#include "robotino.h"
#include "cameraview.h"
#include "audioview.h"
#include "accelerometerview.h"
#include "distancesensorview.h"
#include "powerview.h"

namespace Ui {
    class RobotinoWindow;
}

class RobotinoWindow : public QFrame
{
    Q_OBJECT

public:
    explicit RobotinoWindow(QWidget *parent = 0);
    ~RobotinoWindow();
    Robotino *getRobotino() { return &robotino; }
    void readSettings();

    Ui::RobotinoWindow *ui;

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:

public slots:
    void connectionStateChanged( bool isConnected );

private slots:
    void updateViews();
    void on_ConnectButton_clicked();
    void on_ccl_clicked();
    void on_cl_clicked();
    void on_n_clicked();
    void on_s_clicked();
    void on_o_clicked();
    void on_w_clicked();
    void on_stop_clicked();
    void on_playHigh_clicked();
    void on_playLow_clicked();
    void on_playNoise_clicked();
    void on_playChirpUp_clicked();
    void on_playChirpDown_clicked();
    void on_playClick_clicked();
    void on_fileChoose_clicked();
    void on_record_clicked();
    void on_playPause_clicked();
    void on_playRecordStop_clicked();
    void audioConnection(bool);

private:
  Robotino robotino;
  CameraView *camera;
  AudioView *audio;
  DistanceSensorView *distance;
  AccelerometerView *acceleration;
  PowerView *power;
  QByteArray fileDialogSaveState;
  QFile dataFile, imageFile;
  QTimer updateTimer;
  bool isPaused;

  void writeSettings();
  void updateVelocity();
  int _vx;
  int _vy;
  int _omega;
  int _speed;

};

#endif // ROBOTINOWINDOW_H
