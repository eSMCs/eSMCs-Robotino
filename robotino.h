// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef _ROBOTINO_H_
#define _ROBOTINO_H_

#include <sys/timeb.h>

#include <QtGui>
#include "rec/robotino/com/all.h"
#include "robotinoaudio.h"
#include "robotinocamera.h"
#include "robotinoinfo.h"

using namespace rec::robotino::com;

typedef struct
{
  int a[5];
} Action;

class Robotino : public QObject, public Com
{
  Q_OBJECT

public:
  Robotino( QObject* parent );
  virtual void errorEvent(Error error, const char *errorString);
  virtual void updateEvent();
  void setVelocity(float vx, float vy, float omega);
  void playTone(unsigned tone);
  void setLED(bool);
  const unsigned numDistanceSensors;
  unsigned long getRobotinoTime();
  void lockData();
  void unlockData();

  QVector<double> *getMotorCurrents();
  QVector<double> *getDistances();
  QVector<double> *getAccelerations();
  QVector<double> *getPower();
  void getImage(QImage&);
  QString getMessage();
  bool hasNewMessage();
  QByteArray *getRateMap();
  float getUpdateRate();
  QVector<bool> *getBumper();
  float getvx() {return vx;}
  float getvy() {return vy;}
  float getomega() {return omega;}
  float getsound() {return sound;}
  float getLED() {return LED;}

  void startRecordingData(QFile *, QFile *);
  void stopRecordingData();
  void writeFileHeader(QFile&);
  void saveStateToFile(QFile&);
  void readFileHeader(QFile&);
  void setDataFile(QFile *, QFile *);
  void startReplayingFile();
  void stopReplayingFile();

  QVector<double> motorCurrent[3], epochMotor[3];
  QVector<double> distance[9], epochDistance[9];
  QVector<double> acceleration[3], epochAcceleration[3];
  QVector<double> power[2], epochPower[2];
  QByteArray rateMap[2][8], epochRateMap[2][8];
  QVector<bool> bumperTriggered, epochBumper;
  QVector<QImage> imageSeq, epochImage;
  QList<Action> nextAction;
  Action currentAction, action;
  float vxsoll, vysoll, omegasoll;
  float avgUpdateRate;

public slots:
  void connectToRobot();
  void disconnectFromRobot();
  void onNewCameraImage();

signals:
  void connectionStateChanged( bool isConnected );
  void haveNewFileData();
  void haveNewData(unsigned long);
  void dataFileAtEnd();
  void audioConnection(bool isConnected);
  void breakingNews(QString news);

private:
  void connectionStateChangedEvent( rec::robotino::com::Com::ConnectionState newState, rec::robotino::com::Com::ConnectionState oldState );

  AnalogInput analogInput[3];
  float baseline[3], baselineCandidate[3], count[3];
  DigitalOutput digitalOut[7];
  DistanceSensor distanceSensor[9];
  QPointF distanceSensorHeading[9];
  RobotinoCamera camera;
  RobotinoInfo info;
  OmniDrive omniDrive;
  Motor motor[3];
  PowerManagement powerManagement;
  RobotinoAudio audio;
  Bumper bumper;

  int actionOffset, actionSize, distanceOffset, distanceSize, accelOffset,
    accelSize, currentOffset, currentSize, powerOffset, powerSize, audioOffset,
    audioSize, bumperOffset, bumperSize, recordSize, headerSize;
  float vx, vy, omega, LED, sound;
  QTextStream textStream;
  QDataStream imageStream;
  QTimer readTimer;
  bool isRecording;
  QMutex stopRecordingMutex, epochMutex;
  unsigned long currentTime;

  void saveState(unsigned audioFrames);
  void saveEpochData();

private slots:
  void replayFile_callback();

};

#endif //_ROBOTINO_H_
