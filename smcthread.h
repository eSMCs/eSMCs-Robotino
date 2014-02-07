// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef SMCTHREAD_H
#define SMCTHREAD_H

#include <QByteArray>
#include <QColor>
#include <QFile>
#include <QHash>
#include <QThread>
#include <QVector>

const int HISTORYLENGTH = 20, PREDICTIONDEPTH = 1;
const int ACTIONSIZE=5, NACTIONS=2, OBSERVATIONSIZE=16;

class Robotino;

typedef struct
{
  float value;
  unsigned count;
} ActionValue;

typedef struct SMC
{
  unsigned count, serial;
  float internalValue, externalValue;
  QHash<QByteArray, SMC> nextSMC;
} SMC;

typedef struct SMCPredictionStep
{
  int action;
  double value;
  int count;
  int reliability;
} SMCPredictionStep;

typedef struct SMCPrediction
{
  QVector<SMCPredictionStep> smcSequence;
  QByteArray actionObservationSequence;
} SMCPrediction;

class SMCThread : public QThread
{
  Q_OBJECT


  public:
    SMCThread(QObject *);
    void setRobotino(Robotino *);
    Robotino *getRobotino();
    void loadSMCs(QString);
    void saveSMCs(QString);
    int action2nr(QByteArray);
    QByteArray nr2action(int);

    QByteArray action, observation, nextAction, lastAction, lastObservation;
    QByteArray actionObservationHistory, currentActionObservationHistory, lastActionObservationHistory;
    QHash<QByteArray, SMC> *keytree[HISTORYLENGTH+2];
    SMC rootSMC, *currentSMC[HISTORYLENGTH+1], *lastSMC[HISTORYLENGTH+1];
    QVector<float> internalValue, lastInternalValue;
    float currentValue, anticipatedValue;
    QVector<double> value, reliability, correctPredictionLength, nSMCs;
    QVector<int> count;
    QList<SMCPrediction> prediction;
    QVector<QList<SMCPrediction> > predictionHist;
    QVector<bool> havePredictionForAction;

  public slots:
    void newRawData();
    void newRawFileData();
    void startFeatureComputation(bool);
    void startSMCComputation(bool);
    void clearSMCs();

  signals:
    void newFeatures();
    void newSMC();
    void newAction();
    void message(QColor, QString);

  private:
    void computeFeatures();
    void computeInternalValue();
    void updateSMCs();
    void chooseAction();
    void executeAction();
    void chooseRandomAction();
    void stopAction();
    void buildPredictions();
    double evaluateActionSequence(QByteArray actObsHist, QVector<SMCPredictionStep> actSeq,
                                  int depth);
    double traverseSMCTree(QHash<QByteArray, SMC> nextSMC,
                           QByteArray actObsHist, QVector<SMCPredictionStep> smcSeq,
                           int reliability, int depth);
    void saveNode(const SMC& smc);
    void loadNode(SMC& smc);

    Robotino *robotino;
    int accel[3];
    int treeDepth;
    bool bumper;
    enum {idle, randomExploration, SMCControl, SMCControlSingleStep} mode;
    unsigned long serial;
    QDataStream data;
};

#endif // SMCTHREAD_H
