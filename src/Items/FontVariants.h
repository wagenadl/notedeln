// FontVariants.h

#ifndef FONTVARIANTS_H

#define FONTVARIANTS_H

#include <QFont>
#include <QFontMetricsF>
#include "MarkupStyles.h"

class FontVariants {
public:
  FontVariants(QFont const &);
  FontVariants();
  ~FontVariants();
  void setBase(QFont const &);
  QFont const *font(MarkupStyles);
  QFontMetricsF const *metrics(MarkupStyles);
private:
  static QFont *italicVersion(QFont const *f);
  static QFont *boldVersion(QFont const *f);
  static QFont *scriptVersion(QFont const *f);
private:
  QMap<MarkupStyles, QFont *> fmap;
  QMap<MarkupStyles, QFontMetricsF *> fmmap;
};

#endif
