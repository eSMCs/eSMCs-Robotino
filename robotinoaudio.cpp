// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include <iostream>
#include "robotinoaudio.h"
#define RECVPORT 12121

RobotinoAudio::RobotinoAudio(QObject *parent) :
    QObject(parent),
    sock(parent)
{
}

void RobotinoAudio::connectToAudioServer()
{
  sock.connectToHost("172.26.1.12", 12121);
  sock.write("init");
}

void RobotinoAudio::disconnectFromAudioServer()
{
  sock.write("close");
  sock.disconnectFromHost();
}

void RobotinoAudio::playTone(int tone)
{
  switch(tone)
  {
    case 0: sock.write("play_0"); break;
    case 1: sock.write("play_1"); break;
    case 2: sock.write("play_2"); break;
    case 3: sock.write("play_3"); break;
    case 4: sock.write("play_4"); break;
    case 5: sock.write("play_5"); break;
    case 6: sock.write("play_6"); break;
  }
  sock.flush();
}

bool RobotinoAudio::isConnected()
{
  return sock.isOpen();
}

void RobotinoAudio::capture(QByteArray& buffer)
{
  if(sock.state()==QAbstractSocket::ConnectedState)
  {
    if(sock.hasPendingDatagrams())
    {
      buffer.resize(sock.pendingDatagramSize());
      sock.readDatagram(buffer.data(), buffer.size());
    }
    else
      buffer.clear();
  }
  else
    buffer.clear();
}
