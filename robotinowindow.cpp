// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include "robotinowindow.h"
#include "ui_robotinowindow.h"
#include "cameraview.h"
#include "distancesensorview.h"
#include "accelerometerview.h"
#include "powerview.h"

const QString robotinosIP("172.26.1.12");

RobotinoWindow::RobotinoWindow(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RobotinoWindow),
    robotino(this), updateTimer(), isPaused(false),
    _vx( 0 ), _vy( 0), _omega( 0 ), _speed( 20 )
{
  ui->setupUi(this);

  connect(&robotino, SIGNAL(connectionStateChanged(bool)),
          this, SLOT(connectionStateChanged(bool)));

  camera = new CameraView(&robotino);
  ui->cameraPanel->layout()->addWidget(camera);

  audio = new AudioView(&robotino, 1200, 8);
  ui->audioPanel->layout()->addWidget(audio);

  connect(&robotino, SIGNAL(audioConnection(bool)),
          ui->playHigh, SLOT(setEnabled(bool)));
  connect(&robotino, SIGNAL(audioConnection(bool)),
          ui->playLow, SLOT(setEnabled(bool)));
  connect(&robotino, SIGNAL(audioConnection(bool)),
          ui->playNoise, SLOT(setEnabled(bool)));
  connect(&robotino, SIGNAL(audioConnection(bool)),
          ui->playChirpUp, SLOT(setEnabled(bool)));
  connect(&robotino, SIGNAL(audioConnection(bool)),
          ui->playChirpDown, SLOT(setEnabled(bool)));
  connect(&robotino, SIGNAL(audioConnection(bool)), SLOT(audioConnection(bool)));

  distance = new DistanceSensorView(&robotino, 9);
  ui->DistanceSensors->layout()->addWidget(distance);

  acceleration = new AccelerometerView(&robotino);
  ui->Accelerometer->layout()->addWidget(acceleration);

  power = new PowerView(&robotino, ui->motorPlot, ui->powerPlot);

  connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateViews()));
  ui->bumperDisplay->setAutoFillBackground(true);
}

RobotinoWindow::~RobotinoWindow()
{
  delete power;
  delete ui;
}

void RobotinoWindow::closeEvent(QCloseEvent *event)
{
  robotino.disconnectFromRobot();
  writeSettings();
  event->accept();
}

void RobotinoWindow::on_ConnectButton_clicked()
{
  if( robotino.isConnected() )
    robotino.disconnectFromRobot();
  else
    robotino.connectToRobot();
}

void RobotinoWindow::connectionStateChanged( bool isConnected )
{
  if(isConnected)
  {
    ui->ConnectButton->setText( "Disconnect" );
    updateTimer.start(0);
  }
  else
  {
    ui->ConnectButton->setText( "Connect" );
    robotino.disconnectFromRobot();
    updateTimer.stop();
  }
}

void RobotinoWindow::audioConnection(bool connected)
{
  if(connected)
    ui->messagePanel->append("Connected to audio server");
  else
    ui->messagePanel->append("Disconnected from audio server");
}

void RobotinoWindow::updateViews()
{
  camera->newData();
  audio->newData();
  acceleration->newData();
  distance->newData();
  power->newData();
  if(robotino.hasNewMessage())
    ui->messagePanel->append(robotino.getMessage());
  ui->updateRate->display((int)(robotino.getUpdateRate()+0.5));

  QVector<bool> *bumper = robotino.getBumper();
  if(bumper && !bumper->isEmpty() && bumper->last())
  {
    QPalette bla;
    bla.setColor(QPalette::Window, QColor(200,0,0));
    ui->bumperDisplay->setPalette(bla);
  }
  else
    ui->bumperDisplay->setPalette(QPalette());
}

void RobotinoWindow::updateVelocity()
{
  robotino.vxsoll = _vx;
  robotino.vysoll = _vy;
  robotino.omegasoll = _omega;
}

void RobotinoWindow::on_ccl_clicked()
{
  _omega = robotino.getomega() +  _speed;
  updateVelocity();
}

void RobotinoWindow::on_cl_clicked()
{
  _omega = robotino.getomega() - _speed;
  updateVelocity();
}

void RobotinoWindow::on_n_clicked()
{
  _vx = robotino.getvx() + _speed;
  updateVelocity();
}

void RobotinoWindow::on_s_clicked()
{
  _vx = robotino.getvx() - _speed;
  updateVelocity();
}

void RobotinoWindow::on_w_clicked()
{
  _vy = robotino.getvy() + _speed;
  updateVelocity();
}

void RobotinoWindow::on_o_clicked()
{
  _vy = robotino.getvy() - _speed;
  updateVelocity();
}

void RobotinoWindow::on_stop_clicked()
{
  _vx = 0;
  _vy = 0;
  _omega = 0;
  updateVelocity();
}

void RobotinoWindow::on_playHigh_clicked()
{
  robotino.playTone(2);
}

void RobotinoWindow::on_playLow_clicked()
{
  robotino.playTone(1);
}


void RobotinoWindow::on_playChirpUp_clicked()
{
  robotino.playTone(3);
}

void RobotinoWindow::on_playChirpDown_clicked()
{
  robotino.playTone(4);
}

void RobotinoWindow::on_playNoise_clicked()
{
  robotino.playTone(5);
}

void RobotinoWindow::on_playClick_clicked()
{
  robotino.playTone(6);
}

void RobotinoWindow::on_fileChoose_clicked()
{
  QFileDialog dialog(this);
  dialog.restoreState(fileDialogSaveState);
  QStringList fileNames;
  if (dialog.exec())
  {
    fileNames = dialog.selectedFiles();
    ui->lineEdit->setText(fileNames.first());
    fileDialogSaveState = dialog.saveState();
  }
}

void RobotinoWindow::on_record_clicked()
{
  bool append=false;

  if(ui->record->isChecked())
  {
    dataFile.setFileName(ui->lineEdit->text());
    imageFile.setFileName(ui->lineEdit->text()+".image");
    if(dataFile.exists())
    {
      QMessageBox msgBox;
      msgBox.setText(ui->lineEdit->text());
      msgBox.setInformativeText("Overwrite or Append?");
      QPushButton *overwriteButton = msgBox.addButton(tr("Overwrite"), QMessageBox::ActionRole);
      QPushButton *appendButton = msgBox.addButton(tr("Append"),QMessageBox::RejectRole);
      msgBox.exec();
      if (msgBox.clickedButton() == appendButton)
        append = true;
    }
    if(!append)
    {
      dataFile.open(QIODevice::WriteOnly);
      imageFile.open(QIODevice::WriteOnly);
      robotino.writeFileHeader(dataFile);
    }
    else
    {
      dataFile.open(QIODevice::Append);
      imageFile.open(QIODevice::Append);
    }
    robotino.startRecordingData(&dataFile, &imageFile);
  }
  else
  {
    robotino.stopRecordingData();
    dataFile.close();
    imageFile.close();
  }
}

void RobotinoWindow::on_playPause_clicked()
{
  if(ui->playPause->isChecked())
    if(isPaused)
    {
      robotino.startReplayingFile();
    }
    else
    {
      dataFile.setFileName(ui->lineEdit->text());
      if(dataFile.exists())
      {
        dataFile.open(QIODevice::ReadOnly);
        robotino.readFileHeader(dataFile);
        connect(&robotino,SIGNAL(dataFileAtEnd()), ui->playRecordStop, SLOT(click()));

        imageFile.setFileName(ui->lineEdit->text()+".img");
        if(imageFile.exists())
          imageFile.open(QIODevice::ReadOnly);

        robotino.setDataFile(&dataFile, &imageFile);
        robotino.startReplayingFile();
        updateTimer.start(0);
      }
      else
        ui->playPause->setChecked(false);
    }
  else
  {
    robotino.stopReplayingFile();
    isPaused = true;
    updateTimer.stop();
  }
}

void RobotinoWindow::on_playRecordStop_clicked()
{
  if(ui->record->isChecked())
  {
    ui->record->setChecked(false);
  }
  if(ui->playPause->isChecked())
  {
    robotino.stopReplayingFile();
    ui->playPause->setChecked(false);
    isPaused = false;
    disconnect(&robotino, SIGNAL(haveNewFileData()));
    disconnect(&robotino,SIGNAL(dataFileAtEnd()), ui->playRecordStop, SLOT(click()));
    dataFile.close();
    imageFile.close();
    updateTimer.stop();
    updateViews();
  }
}

void RobotinoWindow::readSettings()
{
  QSettings settings("UKE", "robotino1");
  parentWidget()->setGeometry(settings.value("RobotinoWindow/Geometry", QRect(0, 0, 400, 400)).toRect());
  ui->lineEdit->setText(settings.value("RobotinoWindow/Filename").toString());
  ui->splitter->restoreState(settings.value("RobotinoWindow/Splitter").toByteArray());
}

void RobotinoWindow::writeSettings()
{
  QSettings settings("UKE", "robotino1");
  settings.setValue("RobotinoWindow/Geometry", parentWidget()->geometry());
  settings.setValue("RobotinoWindow/Filename", ui->lineEdit->text());
  settings.setValue("RobotinoWindow/Splitter", ui->splitter->saveState());
}
