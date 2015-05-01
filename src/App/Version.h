#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QDateTime>

namespace Version {
  QString toString();
  QDateTime buildDate();
};

#endif
