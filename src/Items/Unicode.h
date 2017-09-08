// Unicode.h

#ifndef UNICODE_H

#define UNICODE_H

#include <QChar>

namespace Unicode {
  inline bool isCombining(QChar c) { return c>=0x0300 && c<=0x036f; }
  inline bool isHighSurrogate(QChar c) { return c>=0xd800 && c<=0xdbff; }
  inline bool isLowSurrogate(QChar c) { return c>=0xdc00 && c<=0xdfff; }
  inline bool isAnySurrogate(QChar c) { return c>=0xd800 && c<=0xdfff; }
};

#endif
