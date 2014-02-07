// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include "ui_robotinofeaturewindow.h"
#include "robotinofeaturewindow.h"
#include "robotino.h"
#include "smcthread.h"
#include "imageseqview.h"

const int DISPHIST = 200;

RobotinoFeatureWindow::RobotinoFeatureWindow(SMCThread *smcthread, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RobotinoFeatureWindow),
    x(), featX()
{
  ui->setupUi(this);
  this->smcThread = smcthread;

  audio = new AudioView(NULL, 120, 8, ui->audioPanel);
  ui->audioPanel->layout()->addWidget(audio);

  imageSeqView = new ImageSeqView();
  ui->imagePanel->layout()->addWidget(imageSeqView);

  for(int i=0; i<3; i++)
  {
    featAcceleration[i] = QVector<double>();
    featMotorCurrent[i] = QVector<double>();
  }

  for(int i=0;i<9; i++)
  {
    featDistance[i] = QVector<double>();
    distanceCurve[i].attach(ui->distancePlot);
    distanceCurve[i].setPen(QColor(20,255-i*20,40));
    featDistanceCurve[i].attach(ui->featDistancePlot);
    featDistanceCurve[i].setPen(QColor(20,255-i*20,40));
  }
  ui->distancePlot->setAxisMaxMinor(QwtPlot::yLeft, 0);
  ui->distancePlot->enableAxis(QwtPlot::xBottom, false);
  ui->featDistancePlot->enableAxis(QwtPlot::yRight);
  ui->featDistancePlot->enableAxis(QwtPlot::yLeft, false);
  ui->featDistancePlot->setAxisScale(QwtPlot::yRight, 0, 3, 1);
  ui->featDistancePlot->setAxisMaxMinor(QwtPlot::yRight, 0);
  ui->featDistancePlot->enableAxis(QwtPlot::xBottom, false);

  for(int i=0;i<3; i++)
    motorCurve[i].attach(ui->motorCurrentPlot);
  ui->motorCurrentPlot->setAxisScale(QwtPlot::yLeft, 0, 1.0, 0.1);
  ui->motorCurrentPlot->enableAxis(QwtPlot::xBottom, false);
  ui->motorCurrentPlot->setAxisMaxMinor(QwtPlot::yLeft, 0);
  motorCurve[0].setPen(QPen(QColor(200,80,40)));
  motorCurve[1].setPen(QPen(QColor(80,200,40)));
  motorCurve[2].setPen(QPen(QColor(80,40,200)));

  for(int i=0;i<3; i++)
  {
    featMotorCurve[i].attach(ui->featMotorCurrentPlot);
    featMotorCurve[i].setYAxis(QwtPlot::yRight);
  }
  featMotorCurve[0].setPen(QPen(QColor(200,80,40)));
  featMotorCurve[1].setPen(QPen(QColor(80,200,40)));
  featMotorCurve[2].setPen(QPen(QColor(80,40,200)));
  ui->featMotorCurrentPlot->enableAxis(QwtPlot::yRight);
  ui->featMotorCurrentPlot->enableAxis(QwtPlot::yLeft, false);
  ui->featMotorCurrentPlot->enableAxis(QwtPlot::xBottom, false);
  ui->featMotorCurrentPlot->setAxisMaxMinor(QwtPlot::yRight, 0);

  for(int i=0; i<3; i++)
    accelCurve[i].attach(ui->accelerationPlot);
  ui->accelerationPlot->enableAxis(QwtPlot::xBottom, false);
  accelCurve[0].setPen(QPen(QColor(200,200,0)));
  accelCurve[1].setPen(QPen(QColor(0,200,0)));
  accelCurve[2].setPen(QPen(QColor(0,0,200)));

  for(int i=0; i<3; i++)
  {
    featAccelCurve[i].attach(ui->featAccelerationPlot);
    featAccelCurve[i].setYAxis(QwtPlot::yRight);
  }
  featAccelCurve[0].setPen(QPen(QColor(200,200,0)));
  featAccelCurve[1].setPen(QPen(QColor(0,200,0)));
  featAccelCurve[2].setPen(QPen(QColor(0,0,200)));
  ui->featAccelerationPlot->enableAxis(QwtPlot::yRight);
  ui->featAccelerationPlot->enableAxis(QwtPlot::yLeft, false);
  ui->featAccelerationPlot->enableAxis(QwtPlot::xBottom, false);
  ui->featAccelerationPlot->setAxisMaxMinor(QwtPlot::yRight, 0);

  for(int i=0; i<5; i++)
  {
    internalValueData[i] = QVector<double>();
    internalValueCurve[i].attach(ui->internalValuePlot);
    internalValueCurve[i].setYAxis(QwtPlot::yRight);
  }
  internalValueCurve[0].setPen(QPen(QColor(150,150, 40)));
  internalValueCurve[1].setPen(QPen(QColor(110, 110, 110)));
  internalValueCurve[2].setPen(QPen(QColor(200, 80, 80)));
  internalValueCurve[3].setPen(QPen(QColor(40, 20, 150)));
  internalValueCurve[4].setPen(QPen(QColor(40, 200, 50)));
  ui->internalValuePlot->enableAxis(QwtPlot::yRight);
  ui->internalValuePlot->enableAxis(QwtPlot::yLeft, false);
  ui->internalValuePlot->setAxisMaxMinor(QwtPlot::yRight, 0);

  connect(ui->startStopButton, SIGNAL(toggled(bool)), smcThread, SLOT(startFeatureComputation(bool)));
  connect(smcThread, SIGNAL(newFeatures()), SLOT(newActionObservationPair()));
}

RobotinoFeatureWindow::~RobotinoFeatureWindow()
{
  delete audio;
  delete ui;
}

void RobotinoFeatureWindow::updateRawViews()
{
  int i;
  x.clear();
  for(i=0; i<smcThread->getRobotino()->epochMotor[0].size(); i++)
    x.append(i);
  for(i=0; i<9; i++)
    distanceCurve[i].setSamples(x, smcThread->getRobotino()->epochDistance[i]);
  ui->distancePlot->replot();
  for(i=0; i<3; i++)
    motorCurve[i].setSamples(x, smcThread->getRobotino()->epochMotor[i]);
  ui->motorCurrentPlot->replot();

  for(i=0; i<2; i++)
    powerCurve[i].setSamples(x, smcThread->getRobotino()->epochPower[i]);

  for(i=0;i<3;i++)
    accelCurve[i].setSamples(x, smcThread->getRobotino()->epochAcceleration[i]);
  ui->accelerationPlot->replot();

  audio->setData((QByteArray *)(&(smcThread->getRobotino()->epochRateMap[0][0])));
  audio->update();

  imageSeqView->setData(smcThread->getRobotino()->epochImage);
  imageSeqView->update();
}

void RobotinoFeatureWindow::updateFeatureViews()
{
  updateRawViews();

  for(int i=0; i<3; i++)
  {
    featMotorCurve[i].setSamples(featX, featMotorCurrent[i]);
    featAccelCurve[i].setSamples(featX, featAcceleration[i]);
  }
  ui->featMotorCurrentPlot->replot();
  ui->featAccelerationPlot->replot();

  for(int i=0; i<9; i++)
    featDistanceCurve[i].setSamples(featX, featDistance[i]);
  ui->featDistancePlot->replot();

  for(int i=0; i<5; i++)
    internalValueCurve[i].setSamples(featX, internalValueData[i]);
  ui->internalValuePlot->replot();
}

void RobotinoFeatureWindow::newActionObservationPair()
{
  updateRawViews();

  if(featX.isEmpty())
    featX.append(1);
  else
    featX.append(featX.last()+1);
  if(featX.size()>DISPHIST)
    featX.remove(0, featX.size()-DISPHIST);

  for(int i=0; i<9; i++)
  {
    featDistance[i].append((unsigned char)smcThread->observation[7+i]);
    if(featDistance[i].size()>DISPHIST)
      featDistance[i].remove(0, featDistance[i].size()-DISPHIST);
  }

  for(int i=0; i<5; i++)
  {
    internalValueData[i].append(smcThread->internalValue[i]);
    if(internalValueData[i].size()>DISPHIST)
      internalValueData[i].remove(0, internalValueData[i].size()-DISPHIST);
  }

  featMotorCurrent[0].append((unsigned char)smcThread->observation[1]);
  featMotorCurrent[1].append((unsigned char)smcThread->observation[2]);
  featMotorCurrent[2].append((unsigned char)smcThread->observation[3]);
  featAcceleration[0].append((unsigned char)smcThread->observation[4]);
  featAcceleration[1].append((unsigned char)smcThread->observation[5]);
  featAcceleration[2].append((unsigned char)smcThread->observation[6]);
  for(int i=0; i<3; i++)
  {
    if(featMotorCurrent[i].size()>DISPHIST)
      featMotorCurrent[i].remove(0, featMotorCurrent[i].size()-DISPHIST);
    if(featAcceleration[i].size()>DISPHIST)
      featAcceleration[i].remove(0, featAcceleration[i].size()-DISPHIST);
  }

  updateFeatureViews();
}

void RobotinoFeatureWindow::readSettings()
{
  QSettings settings("UKE", "robotino1");
  parentWidget()->setGeometry(settings.value("RobotinoFeatureWindow/Geometry", QRect(0, 0, 400, 400)).toRect());
}

void RobotinoFeatureWindow::writeSettings()
{
  QSettings settings("UKE", "robotino1");
  settings.setValue("RobotinoFeatureWindow/Geometry", parentWidget()->geometry());
}

void RobotinoFeatureWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}
