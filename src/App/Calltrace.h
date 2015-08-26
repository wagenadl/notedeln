// Calltrace.h

#ifndef CALLTRACE_H

#define CALLTRACE_H

#include <QString>
#include <QStringList>

namespace Calltrace {
  QStringList list();
  QString quick();
  QString full(int skip);
};  

#endif
