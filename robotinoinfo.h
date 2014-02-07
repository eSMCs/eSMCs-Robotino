// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#ifndef ROBOTINOINFO_H
#define ROBOTINOINFO_H

#include "rec/robotino/com/Info.h"
#include <QString>

class RobotinoInfo : public rec::robotino::com::Info
{
public:
  virtual void infoReceivedEvent (const char *text);
  bool messageIsNew;

  QString message;
};

#endif // ROBOTINOINFO_H
