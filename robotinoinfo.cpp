// eSMCs Robotino
// The eSMCs project, EU grant no 270212, esmcs.eu
// Alexander Maye, University Medical Center Hamburg-Eppendorf
// 01/05/2014

#include "robotinoinfo.h"

void RobotinoInfo::infoReceivedEvent (const char *text)
{
  message = text;
  messageIsNew = true;
}
