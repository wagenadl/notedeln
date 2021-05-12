// Items/TextItemDocData.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  QFont baseFont; // do not set directly, use access function instead:
  void setBaseFont(QFont const &);
  double indent;
  double width;
  double leftmargin;
  double rightmargin;
  double lineheight;
  QColor color;
  QRectF br;
  bool writable;
  QVector<QPointF> linepos;
  double ascent;
  double xheight;
  double descent;
  double y0;
public:
  TextItemDocData(TextData *text);
  QVector<double> const &charWidths() const;
  void forgetWidths() { charwidths.clear(); }
  // map will contain Normal, Italic, Bold, and Superscript and combinations
  void recalcSomeWidths(int start=0, int end=-1) const;
  void setCharWidths(QVector<double> const &);
  FontVariants &fonts() const { return fv; }
  double italicCorrection(class MarkupStyles const &) const;
private:
  mutable QMap<MarkupStyles, QFontMetricsF> mtr;
  mutable QVector<double> charwidths;
  mutable FontVariants fv;
};

#endif
