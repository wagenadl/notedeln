// Unicode.cpp

#include "Unicode.h"

namespace Unicode {
  bool isCombining(QChar c) {
    return (c>=0x0300 && c<=0x036f)
      || (c>=0x1ab0 && c<=0x1aff)
      || (c>=0x1dc0 && c<=0x1dff)
      || (c>=0x20d0 && c<=0x20ff)
      || (c>=0xfe20 && c<=0xfe2f)
      || c==0x3099 || c==0x309a;
  }
};

