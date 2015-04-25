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
#include "MarkupStyles.h"

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
  // map will contain Normal, Italic, Bold, and Superscript and combinations
  void recalcSomeWidths(int start=0, int end=-1) const;
  void setCharWidths(QVector<double> const &);
  FontVariants &fonts() const { return fv; }
private:
  mutable QMap<MarkupStyles, QFontMetricsF> mtr;
  mutable QVector<double> charwidths;
  mutable FontVariants fv;
};

#endif
