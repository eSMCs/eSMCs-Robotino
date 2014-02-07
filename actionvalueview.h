// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ACTIONVALUEVIEW_H
#define ACTIONVALUEVIEW_H

#include <QWidget>
#include <QtGui>
#include "smcthread.h"

struct ActionViewData{
  QColor color;
  QPointF movementdir;
  int rotation, width;
};

class ActionValueView : public QWidget
{
  Q_OBJECT
public:
  explicit ActionValueView(QWidget *parent = 0);
  void newData(QVector<double> value, QVector<double> reliability);

signals:

public slots:

private:
  void paintEvent( QPaintEvent* e );
  QPen actionPen;
  QImage image;
  QVector<ActionViewData> viewData;
};

#endif // ACTIONVALUEVIEW_H
