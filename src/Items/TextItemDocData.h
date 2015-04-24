// TextItemDocData.h

#ifndef TEXTITEMDOCDATA_H

#define TEXTITEMDOCDATA_H

#include "TextData.h"
#include <QVector>
#include <QFontMetricsF>
#include <QMap>
#include <QColor>
#include <QRectF>
#include "FontVariants.h"

class TextItemDocData {
public:
  TextData *text;
  QVector<int> linestarts;
  QFont baseFont; // do not set directly
  void setBaseFont(QFont const &);
  double indent;
  double width;
  double lineheight;
  QColor color;
  QRectF br;
public:
  TextItemDocData(TextData *text): text(text) { }
  QVector<double> const &charWidths() const;
  void forgetWidths() { charwidths.clear(); }
  QMap<MarkupData::Styles, QFontMetricsF> const &metrics() const;
  // map will contain Normal, Italic, Bold, and Superscript and combinations
  void recalcSomeWidths(int start=0, int end=-1) const;
  void setCharWidths(QVector<double> const &);
private:
  mutable QMap<MarkupData::Styles, QFontMetricsF> mtr;
  mutable QVector<double> charwidths;
  mutable FontVariants fv;
};

#endif
