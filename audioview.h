// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef AUDIOVIEW_H
#define AUDIOVIEW_H

#include <QWidget>

class Robotino;

class AudioView : public QWidget
{
    Q_OBJECT
public:
    explicit AudioView(Robotino *, int nsamples, int nfreqbands, QWidget *parent = 0);
    void newData(); // gets data from Robotino
    void setData(QByteArray *);

signals:

public slots:

private:
    void paintEvent( QPaintEvent* e );
    QImage imageLeft, imageRight;
    Robotino *robot;
    const int NSAMPLES, NFREQBANDS;
};

#endif // AUDIOVIEW_H
