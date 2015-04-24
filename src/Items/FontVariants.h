// FontVariants.h

#ifndef FONTVARIANTS_H

#define FONTVARIANTS_H

#include <QFont>
#include <QFontMetricsF>
#include "MarkupData.h"

class FontVariants {
public:
  FontVariants(QFont const &);
  FontVariants();
  ~FontVariants();
  void setBase(QFont const &);
  QFont const *font(MarkupData::Styles);
  QFontMetricsF const *metrics(MarkupData::Styles);
private:
  static MarkupData::Styles simplifiedStyle(MarkupData::Styles);
  static QFont *italicVersion(QFont const *f);
  static QFont *boldVersion(QFont const *f);
  static QFont *scriptVersion(QFont const *f);
private:
  QMap<MarkupData::Styles, QFont *> fmap;
  QMap<MarkupData::Styles, QFontMetricsF *> fmmap;
};

#endif
