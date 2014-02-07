// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ACTIONOBSERVATIONVIEW_H
#define ACTIONOBSERVATIONVIEW_H

#include <QtGui>

class ActionObservationView : public QWidget
{
    Q_OBJECT
public:
    explicit ActionObservationView(QWidget *parent = 0);
    void newData(float[], QByteArray, QByteArray, float);

protected:
    virtual void paintEvent( QPaintEvent* e );

private:
    QBrush brush;
    QPen pen, currentActionPen, nextActionPen;
    QImage image;
    QVector<QPointF> heading;
    float data[9], currSpeed[3], nextSpeed[3], value;
signals:

public slots:

};

#endif // ACTIONOBSERVATIONVIEW_H
