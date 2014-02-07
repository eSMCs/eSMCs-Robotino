// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include "MyCom.h"
#include <QtDebug>
#include <iostream>
#include "rec/robotino/com/DigitalOutput.h"

rec::robotino::com::DigitalOutput digitalOut;

MyCom::MyCom( QObject* parent )
: QObject( parent )
, rec::robotino::com::Com(false) //events will be emitted when calling rec::robotino::com::Com::processEvents
, _timer( new QTimer( this ) )
{
        setMinimumUpdateCycleTime( 100 );
        digitalOut.setComId(id());
        digitalOut.setOutputNumber(0);
        digitalOut.setValue(true);
}

void MyCom::errorEvent(Error error, const char *errorString)
{
  std::cout << errorString << std::endl;
}

void MyCom::connectToHost( const QString& address )
{
	if( isConnected() )
	{
                qDebug() << QString("Already connected to ") << rec::robotino::com::Com::address();
		return;
	}

	rec::robotino::com::Com::setAddress( address.toLatin1().constData() );

	rec::robotino::com::Com::connect( false );
        digitalOut.setValue(true);
}

void MyCom::disconnectFromHost()
{
  rec::robotino::com::Com::disconnect();
  digitalOut.setValue(false);
}

void MyCom::on_timer_timeout()
{
  processEvents();
}

void MyCom::connectionStateChangedEvent( rec::robotino::com::Com::ConnectionState newState, rec::robotino::com::Com::ConnectionState oldState )
{
  if( rec::robotino::com::Com::Connected == newState )
    emit connectionStateChanged( true );
  else if( rec::robotino::com::Com::NotConnected == newState )
    emit connectionStateChanged( false );
}

void MyCom::updateEvent()
{
  emit sensorStateChanged();
}
