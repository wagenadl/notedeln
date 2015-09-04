// Unicode.h

#ifndef UNICODE_H

#define UNICODE_H

#include <QChar>

namespace Unicode {
  inline bool isCombining(QChar c) { return c>=0x0300 && c<=0x036f; }
};

#endif
