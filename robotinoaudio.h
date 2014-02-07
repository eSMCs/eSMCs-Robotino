// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOAUDIO_H
#define ROBOTINOAUDIO_H

#include <QObject>
#include <QUdpSocket>

class RobotinoAudio : public QObject
{
    Q_OBJECT
public:
    explicit RobotinoAudio(QObject *parent = 0);

    void connectToAudioServer();
    void disconnectFromAudioServer();
    void playTone(int);
    void capture(QByteArray&);
    bool isConnected();

signals:

public slots:
private:
    QUdpSocket sock;
};

#endif // ROBOTINOAUDIO_H
