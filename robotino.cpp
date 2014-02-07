// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include "robotino.h"
#include <QtDebug>
#include <iostream>
#include "rec/robotino/com/DigitalOutput.h"
#include "cv.h"

const int EPOCHLENGTH = 10, MAXACCEL = 50, MAXV = 150;

Robotino::Robotino( QObject* parent )
: QObject( parent ),
Com(false),
numDistanceSensors(9), vxsoll(0), vysoll(0), omegasoll(0),
avgUpdateRate(0.0),
audio(parent), vx(0), vy(0), omega(0), isRecording(false)
{
  setMinimumUpdateCycleTime( 100 );

  action.a[0] = action.a[1] = action.a[2] = 5;
  action.a[3] = action.a[4] = 0;
  currentAction = action;

  for(unsigned i=0; i<3;i++)
  {
    analogInput[i].setComId(id());
    analogInput[i].setInputNumber(i);
    baseline[i] = baselineCandidate[i] = count[i] = 0;
    acceleration[i] = QVector<double>(120);
    epochAcceleration[i] = QVector<double>(EPOCHLENGTH);
  }

  for(int i=0; i<7; i++)
  {
    digitalOut[i].setComId(id());
    digitalOut[i].setOutputNumber(i);
  }

  for(unsigned i=0; i<numDistanceSensors;i++)
  {
    distanceSensor[i].setComId(id());
    distanceSensor[i].setSensorNumber(i);
    distanceSensorHeading[i].setX(cos((360-distanceSensor[i].heading()-90.0)/360.0*2*M_PI));
    distanceSensorHeading[i].setY(sin((360-distanceSensor[i].heading()-90.0)/360.0*2*M_PI));
    distance[i] = QVector<double>(120);
    epochDistance[i] = QVector<double>(EPOCHLENGTH);
  }

  camera.setComId(id());
  QObject::connect(&camera, SIGNAL(newCameraImage()), SLOT(onNewCameraImage()));
  camera.setStreaming(true);

  info.setComId(id());
  omniDrive.setComId(id());
  for(unsigned i=0; i<3;i++)
  {
    motor[i].setComId(id());
    motor[i].setMotorNumber(i);
    motorCurrent[i] = QVector<double>(120);
    epochMotor[i] = QVector<double>(EPOCHLENGTH);
  }

  powerManagement.setComId(id());
  for(unsigned i=0; i<2; i++)
  {
    power[i] = QVector<double>(120);
    epochPower[i] = QVector<double>(EPOCHLENGTH);
  }

  bumper.setComId(id());
  bumperTriggered = QVector<bool>(120);
  epochBumper = QVector<bool>(EPOCHLENGTH);

  for(unsigned ch=0; ch<2; ch++)
    for(unsigned f=0; f<8; f++)
    {
      rateMap[ch][f] = QByteArray(1400, 0);
      epochRateMap[ch][f] = QByteArray(EPOCHLENGTH*12, 0);
    }

  currentTime = 0;
  info.messageIsNew = false;
}

void Robotino::startRecordingData(QFile *dataFile, QFile *imageFile)
{
  textStream.setDevice(dataFile);
  if(imageFile)
    imageStream.setDevice(imageFile);
  isRecording = true;
}

void Robotino::stopRecordingData()
{
  stopRecordingMutex.lock();
  isRecording = false;
  stopRecordingMutex.unlock();
}

void Robotino::writeFileHeader(QFile& file)
{
  file.write("action:5\n");
  file.write("distance:9\n");
  file.write("accel:3\n");
  file.write("current:3\n");
  file.write("power:2\n");
  file.write("bumper:1\n");
  file.write("audio:16\n");
  file.write("end\n");
  file.flush();
}

void Robotino::saveEpochData()
{
  if(!textStream.device()->isOpen())
    return;

  for(int j=0; j<9; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream << epochDistance[j][i] << " ";

  for(int j=0; j<3; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream << epochMotor[j][i] << " ";

  for(int j=0; j<3; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream << epochAcceleration[j][i] << " ";

  for(int j=0; j<2; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream << epochPower[j][i] << " ";

  for(int i=0; i<EPOCHLENGTH; i++)
    textStream << epochBumper[i] << " ";

  for(unsigned ch=0; ch<2; ch++)
    for(unsigned f=0; f<8; f++)
      for(int i=0; i<EPOCHLENGTH*12; i++)
        textStream << (ushort)epochRateMap[ch][f][i] << " ";

  for(int i=0; i<5; i++)
    textStream << action.a[i] << " ";

  textStream << endl;

  if(!imageStream.device()->isOpen())
    return;

  imageStream << epochImage.size();
  for(int i=0; i<epochImage.size(); i++)
  {
    imageStream << epochImage[i].width() << epochImage[i].height();
    imageStream.writeBytes((const char*)epochImage[i].bits(), epochImage[i].byteCount());
  }
}

void Robotino::readFileHeader(QFile& file)
{
  unsigned buflen = 20;
  char buffer[buflen];

  recordSize = 0;
  file.seek(0);
  file.readLine(buffer, buflen);
  while(!file.atEnd() && strncmp(buffer,"end", strlen("end"))!=0)
  {
    if(strncmp(buffer,"action:", strlen("action:"))==0)
    {
      actionOffset = recordSize;
      actionSize = atoi(buffer+strlen("action:"));
      recordSize += actionSize;
    }
    else if(strncmp(buffer,"distance:", strlen("distance:"))==0)
    {
      distanceOffset = recordSize;
      distanceSize = atoi(buffer+strlen("distance:"));
      recordSize += distanceSize;
    }
    else if(strncmp(buffer,"accel:", strlen("accel:"))==0)
    {
      accelOffset = recordSize;
      accelSize = atoi(buffer+strlen("accel:"));
      recordSize += accelSize;
    }
    else if(strncmp(buffer,"current:", strlen("current:"))==0)
    {
      currentOffset = recordSize;
      currentSize = atoi(buffer+strlen("current:"));
      recordSize += currentSize;
    }
    else if(strncmp(buffer,"power:", strlen("power:"))==0)
    {
      powerOffset = recordSize;
      powerSize = atoi(buffer+strlen("power:"));
      recordSize += powerSize;
    }
    else if(strncmp(buffer,"bumper:", strlen("bumper:"))==0)
    {
      bumperOffset = recordSize;
      bumperSize = atoi(buffer+strlen("bumper:"));
      recordSize += bumperSize;
    }
    else if(strncmp(buffer,"audio:", strlen("audio:"))==0)
    {
      audioOffset = recordSize;
      audioSize = atoi(buffer+strlen("audio:"));
      recordSize += audioSize;
    }
    file.readLine(buffer, buflen);
  }
  headerSize = file.pos();
  //std::cout << "Fertig" << std::endl;
}

void Robotino::startReplayingFile()
{
  QObject::connect(&readTimer,SIGNAL(timeout()),SLOT(replayFile_callback()));
  readTimer.setInterval(0);
  readTimer.start();
}

void Robotino::setDataFile(QFile *file, QFile *iFile)
{
  textStream.setDevice(file);
  imageStream.setDevice(iFile);
}

void Robotino::stopReplayingFile()
{
  readTimer.stop();
  readTimer.disconnect(SIGNAL(timeout()), this, SLOT(replayFile_callback()));
}


void Robotino::replayFile_callback()
{
  static struct timeb then;
  struct timeb now;
  ftime(&now);
  unsigned deltaT = (now.time-then.time)*1000 + (now.millitm-then.millitm);
  avgUpdateRate = 1.0/(deltaT/1000.0);
  then = now;

  if(!textStream.device()->isOpen())
    return;
  for(int j=0; j<9; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
    {
      textStream >> epochDistance[j][i];
      if(textStream.atEnd())
      {
        emit dataFileAtEnd();
        std::cout << "File at end" << std::endl;
        return;
      }
  }

  for(int j=0; j<3; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream >> epochMotor[j][i];

  for(int j=0; j<3; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream >> epochAcceleration[j][i];

  for(int j=0; j<2; j++)
    for(int i=0; i<EPOCHLENGTH; i++)
      textStream >> epochPower[j][i];

  int b;
  for(int i=0; i<EPOCHLENGTH; i++)
  {
    textStream >> b;
    epochBumper[i] = (bool)b;
  }

  ushort c;
  for(unsigned ch=0; ch<2; ch++)
    for(unsigned f=0; f<8; f++)
      for(int i=0; i<EPOCHLENGTH*12; i++)
      {
        textStream >> c;
        epochRateMap[ch][f][i] = c;
      }

  for(int i=0; i<5; i++)
  {
    textStream >> action.a[i];//a;
  }
  action.a[4] = 0;

  emit haveNewFileData();
}

void Robotino::errorEvent(Error , const char *errorString)
{
  std::cout << errorString << std::endl;
}

void Robotino::connectToRobot()
{
  if( isConnected() )
  {
    qDebug() << QString("Already connected to ") << rec::robotino::com::Com::address();
    return;
  }
  setAddress("172.26.1.12");
  rec::robotino::com::Com::connect( false );

  audio.connectToAudioServer();
  if(audio.isConnected())
    emit audioConnection(true);
  else
    emit audioConnection(false);
}

void Robotino::disconnectFromRobot()
{
  audio.disconnectFromAudioServer();
  rec::robotino::com::Com::disconnect();
}

void Robotino::onNewCameraImage()
{
  QImage img;
  camera.getImage(img);
  cv::Mat src(img.height(), img.width(), CV_8UC3, img.bits());
  cv::Mat dst(img.height()/8, img.width()/8, CV_8UC3);
  cv::resize(src, dst, dst.size());
  QImage dest = QImage(dst.ptr(), dst.cols, dst.rows, QImage::Format_RGB888);
  imageSeq.append(dest.copy());
}

void Robotino::connectionStateChangedEvent( rec::robotino::com::Com::ConnectionState newState, rec::robotino::com::Com::ConnectionState )
{
  if( rec::robotino::com::Com::Connected == newState )
    emit connectionStateChanged( true );
  else if( rec::robotino::com::Com::NotConnected == newState )
    emit connectionStateChanged( false );
}

void Robotino::playTone(unsigned tone)
{
  audio.playTone(tone);
  sound = tone;
}

void Robotino::updateEvent()
{
  static int refractoryCounter = 0;
  static struct timeb then;
  struct timeb now;
  ftime(&now);
  unsigned deltaT = (now.time-then.time)*1000 + (now.millitm-then.millitm);
  avgUpdateRate = 1.0/(deltaT/1000.0);
  then = now;

  if(refractoryCounter-->0)
  {
    return;
  }

  // Power management
  for(unsigned i=0; i<2; i++)
    if(power[i].size()>110)
      power[i].remove(0,power[i].size()-110);
  power[0].append(powerManagement.voltage());
  power[1].append(powerManagement.current());

  // Motors
  for(unsigned i=0;i<3;i++)
  {
    if(motorCurrent[i].size()>110)
      motorCurrent[i].remove(0,motorCurrent[i].size()-110);
    motorCurrent[i].append(motor[i].motorCurrent());
  }
  for(unsigned i=0; i<numDistanceSensors; i++)
  {
    if(distance[i].size()>110)
      distance[i].remove(0,distance[i].size()-110);

    float vi = distanceSensor[i].voltage();
    if(vi<0.3)
      distance[i].append(1);
    else
      distance[i].append(log(distanceSensor[i].voltage()/2)/log(0.3/2));
  }

  //Accelerometers
  for(unsigned i=0; i<3; i++)
  {
    float vi = analogInput[i].value();
    if(acceleration[i].size() > 110)
      acceleration[i].remove(0,acceleration[i].size()-110);
    acceleration[i].append(vi);// - baseline[i];
  }

  if(bumperTriggered.size() > 110)
    bumperTriggered.remove(0,bumperTriggered.size()-110);
  bumperTriggered.append(bumper.value());
  if(bumper.value())
  {
    digitalOut[1].setValue(true);
  }
  else
    digitalOut[1].setValue(false);

  for(unsigned ch=0; ch<2; ch++)
    for(unsigned f=0; f<8; f++)
      if(rateMap[ch][f].size() > 1320) // more than 11 seconds
        rateMap[ch][f].remove(0, rateMap[ch][f].size()-1200);

  unsigned audioFrames = 0;
  if(audio.isConnected())
  {
    QByteArray data;
    audio.capture(data);
    while(!data.isEmpty())
    {
      unsigned nframes = data.size()/12/8/2; // 12 samples per frame, 8 filters, 2 channels
      audioFrames += nframes;
      for(unsigned ch=0; ch<2; ch++)
        for(unsigned f=0; f<8; f++)
          for(unsigned frame=0; frame<nframes; frame++)
            for(unsigned sample=0; sample<12; sample++)
              rateMap[ch][f].append(data[((frame*2+ch)*8+f)*12+sample]);
      audio.capture(data);
    }
  }
  currentTime++;

  if(currentTime%EPOCHLENGTH == 0)
  {
    epochMutex.lock();
    for(int i=0; i<EPOCHLENGTH; i++)
    {
      for(int j=0; j<9; j++)
        epochDistance[j][i] = distance[j][distance[j].size()-EPOCHLENGTH+i];
      for(int j=0; j<3; j++)
      {
        epochMotor[j][i] = motorCurrent[j][motorCurrent[j].size()-EPOCHLENGTH+i];
        epochAcceleration[j][i] = acceleration[j][acceleration[j].size()-EPOCHLENGTH+i];
      }
      for(int j=0; j<2; j++)
        epochPower[j][i] = power[j][power[j].size()-EPOCHLENGTH+i];

      epochBumper[i] = bumperTriggered[bumperTriggered.size()-EPOCHLENGTH+i];

    }
    for(unsigned ch=0; ch<2; ch++)
      for(unsigned f=0; f<8; f++)
        for(int i=0; i<EPOCHLENGTH*12; i++) // 12 samples audio per update (100ms)
          epochRateMap[ch][f][i] = rateMap[ch][f][rateMap[ch][f].size()-EPOCHLENGTH*12+i];

    epochImage.clear();
    while(!imageSeq.isEmpty())
    {
      epochImage.append(imageSeq.first());
      imageSeq.remove(0);
    }

    action = currentAction;

    if(isRecording)
      saveEpochData();
    epochMutex.unlock();

    emit haveNewData(currentTime);

    if(!nextAction.isEmpty())
    {
      currentAction = nextAction.takeFirst();
      vxsoll = MAXV*(currentAction.a[0]-5);
      vysoll = MAXV*(currentAction.a[1]-5);
      digitalOut[0].setValue(currentAction.a[4]);
    }
    else
    {
    }
  }
  float newvx = vxsoll;
  if(vxsoll-vx>MAXACCEL)
    newvx = vx + MAXACCEL;
  else if(vxsoll-vx<-MAXACCEL)
    newvx = vx - MAXACCEL;

  float newvy = vysoll;
  if(vysoll-vy>MAXACCEL)
    newvy = vy + MAXACCEL;
  else if(vysoll-vy<-MAXACCEL)
    newvy = vy - MAXACCEL;
    setVelocity(vxsoll, vysoll, currentAction.a[2]-5);
}

QVector<double> *Robotino::getMotorCurrents() {return motorCurrent;}
QVector<double> *Robotino::getDistances() {return distance;}
QVector<double> *Robotino::getAccelerations() {return acceleration;}
QVector<double> *Robotino::getPower(){return power;}
void Robotino::getImage(QImage& img){camera.getImage(img);}
QString Robotino::getMessage() {info.messageIsNew = false; return info.message;}
bool Robotino::hasNewMessage() { return info.messageIsNew;}
QByteArray *Robotino::getRateMap() {return (QByteArray *)rateMap;}
float Robotino::getUpdateRate() {return avgUpdateRate;}
QVector<bool> *Robotino::getBumper() {return &bumperTriggered;}

void Robotino::setVelocity(float vx, float vy, float omega)
{
  omniDrive.setVelocity(vx, vy, omega);
  this->vx = vx;
  this->vy = vy;
  this->omega = omega;
}

void Robotino::setLED(bool onoff)
{
  digitalOut[0].setValue(onoff);
  LED = onoff;
}

unsigned long Robotino::getRobotinoTime() {return currentTime;};
