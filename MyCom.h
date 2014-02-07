// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef _MYCOM_H_
#define _MYCOM_H_

#include <QtGui>
#include "rec/robotino/com/Com.h"

class MyCom : public QObject, public rec::robotino::com::Com
{
  Q_OBJECT

public:
  MyCom( QObject* parent );
  virtual void errorEvent(Error error, const char *errorString);
  virtual void updateEvent();

public slots:
  void connectToHost( const QString& address );
  void disconnectFromHost();

signals:
  void connectionStateChanged( bool isConnected );
  void sensorStateChanged();

private:
  /**
  This function overwrites rec::robotino::com::Com::connectionStateChangedEvent
  */
  void connectionStateChangedEvent( rec::robotino::com::Com::ConnectionState newState, rec::robotino::com::Com::ConnectionState oldState );

  QTimer* _timer;

private slots:
  void on_timer_timeout();
};

#endif //_MYCOM_H_
