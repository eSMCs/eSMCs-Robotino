// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>

#include "smcwindow.h"
#include "ui_smcwindow.h"
#include <QFileDialog>
#include <QErrorMessage>
#include "actionobservationview.h"
#include "actionvalueview.h"
#include "smcthread.h"
#include "robotino.h"

SMCWindow::SMCWindow(SMCThread *smcThread, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SMCWindow),
  reliabilityHistogram("Prediction reliability")
{
  ui->setupUi(this);
  this->smcThread = smcThread;
  connect(ui->startStopButton, SIGNAL(toggled(bool)), smcThread, SLOT(startSMCComputation(bool)));
  connect(ui->clearButton, SIGNAL(clicked()), smcThread, SLOT(clearSMCs()));
  connect(smcThread, SIGNAL(message(QColor,QString)), SLOT(showMessage(QColor, QString)));

  aoView = new ActionObservationView();
  ui->actionValueBox->layout()->addWidget(aoView);

  avView = new ActionValueView();
  ui->actionValueBox->layout()->addWidget(avView);

  reliabilityHistogram.attach(ui->reliabilityPlot);
  reliabilityHistogram.setBrush(QBrush(QColor(Qt::green)));
  ui->reliabilityPlot->setAxisScale(QwtPlot::xBottom , -0.5, HISTORYLENGTH+0.5, 1);
  ui->reliabilityPlot->setAxisMaxMinor(QwtPlot::xBottom, 0);
  ui->reliabilityPlot->setAxisScale(QwtPlot::yLeft, 0, 1, 0.1);
  ui->reliabilityPlot->setAxisMaxMinor(QwtPlot::yLeft, 0);

  predictionCurve.setPen(QPen(QColor(Qt::green)));
  predictionCurve.attach(ui->predictionPlot);
  ui->predictionPlot->setAxisScale(QwtPlot::yLeft, 0, 4, 1);

  for(int i=0;i<NACTIONS; i++)
    x.append(i);

  connect(smcThread, SIGNAL(newSMC()), SLOT(updateVisualization()));
  connect(smcThread, SIGNAL(newAction()), SLOT(updateActionValueDisplay()));
}

SMCWindow::~SMCWindow()
{
  delete ui;
}

void SMCWindow::updateActionValueDisplay()
{
  QVector<QwtIntervalSample> data(HISTORYLENGTH);
  for(int i=0; i<HISTORYLENGTH; i++)
  {
    data[i] =  QwtIntervalSample(smcThread->nSMCs[i], i-0.5, i+0.5);
  }

  reliabilityHistogram.setSamples(data);
  ui->reliabilityPlot->replot();

  if(x.isEmpty())
    x.append(1);
  else
    x.append(x.last()+1);
  if(x.size()>200)
    x.remove(0, x.size()-200);

  if(smcThread->correctPredictionLength.size()<200)
    predictionCurve.setSamples(x, smcThread->correctPredictionLength);
  else
    predictionCurve.setSamples(x, smcThread->correctPredictionLength.mid(
                                 smcThread->correctPredictionLength.size()-200));
  ui->predictionPlot->replot();

  QVector<double> reliability(smcThread->reliability.size());
  QVector<double> value(smcThread->value.size());
  for(int i=0; i<smcThread->reliability.size(); i++)
  {
    value[i] = smcThread->value[i]/smcThread->count[i];
    reliability[i] = smcThread->reliability[i]/HISTORYLENGTH;
  }
  avView->newData(value, reliability);
}

void SMCWindow::updateVisualization()
{
  float currSpeed[3];
  currSpeed[0] = smcThread->getRobotino()->getvx()/150; // max nr of accelerations is 3,
  currSpeed[1] = smcThread->getRobotino()->getvy()/150; // accel per step is 5
  currSpeed[2] = smcThread->getRobotino()->getomega();
  aoView->newData(currSpeed, smcThread->nextAction.left(3), smcThread->observation.mid(7,9),
                  smcThread->anticipatedValue);
}

void SMCWindow::showMessage(QColor color, QString message)
{
  ui->textBox->setTextColor(color);
  ui->textBox->append(message);
}

void SMCWindow::on_fileChooseButton_clicked()
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

void SMCWindow::on_loadButton_clicked()
{
  smcThread->loadSMCs(ui->lineEdit->text());
}

void SMCWindow::on_saveButton_clicked()
{
  smcThread->saveSMCs(ui->lineEdit->text());
}

void SMCWindow::readSettings()
{
  QSettings settings("UKE", "robotino1");
  parentWidget()->setGeometry(settings.value("SMCWindow/Geometry", QRect(10, 10, 400, 400)).toRect());
  ui->lineEdit->setText(settings.value("SMCWindow/Filename").toString());
}

void SMCWindow::writeSettings()
{
  QSettings settings("UKE", "robotino1");
  settings.setValue("SMCWindow/Geometry", parentWidget()->geometry());
  settings.setValue("SMCWindow/Filename", ui->lineEdit->text());
}

void SMCWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}
