// Unicode.h

#ifndef UNICODE_H

#define UNICODE_H

#include <QChar>

namespace Unicode {
  bool isCombining(QChar c);
  inline bool isHighSurrogate(QChar c) { return c>=0xd800 && c<=0xdbff; }
  inline bool isLowSurrogate(QChar c) { return c>=0xdc00 && c<=0xdfff; }
  inline bool isAnySurrogate(QChar c) { return c>=0xd800 && c<=0xdfff; }
  inline bool isSecondary(QChar c) { return isLowSurrogate(c)
      || isCombining(c); }
};

#endif
