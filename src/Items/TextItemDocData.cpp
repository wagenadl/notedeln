// Items/TextItemDocData.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// TextItemDocData.cpp

#include "TextItemDocData.h"
#include "MarkupEdges.h"
#include "Unicode.h"
#include <QDebug>

TextItemDocData::TextItemDocData(TextData *text): text(text) {
  indent = 0;
  width = 1000; // hmm
  leftmargin = rightmargin = 0;
  lineheight = 15; // hmm
  y0 = 4;
  writable = false;
}

void TextItemDocData::setBaseFont(QFont const &f) {
  forgetWidths();
  baseFont = f;
  fv.setBase(f);
  ascent = fv.metrics(MarkupStyles())->ascent();
  xheight = fv.metrics(MarkupStyles())->xHeight();
  descent = fv.metrics(MarkupStyles())->descent();
}

void TextItemDocData::setCharWidths(QVector<double> const &cw) {
  charwidths = cw;
}

QVector<double> const &TextItemDocData::charWidths() const {
  if (charwidths.isEmpty())
    recalcSomeWidths(0, -1);
  return charwidths;
}

void TextItemDocData::recalcSomeWidths(int start, int end) const {
  /* Calculates widths for every character in range. */
  /* If we currently don't have _any_ widths, we calculate whole doc. */
  /* Currently does not yet do italics correction, but it will. */

  QString txt = text->text();
  
  if (charwidths.isEmpty()) {
    start = 0;
    end = -1;
  }
  if (start>0) {
    --start;
    while (start>=0
	   && (Unicode::isLowSurrogate(txt[start])
	       || Unicode::isCombining(txt[start])))
      -- start;
  }

  MarkupStyles current;
  MarkupEdges edges(text->markups());
  foreach (int k, edges.keys()) 
    if (k<start)
      current = edges[k];
    else
      break;
  
  QFontMetricsF const *fm = fv.metrics(current);
  
  int N = txt.size();
  charwidths.resize(N);
  if (end<0)
    end = N;

  for (int n=start; n<end; n++) {
    QChar c = txt[n];
    QString s(c);
    if (Unicode::isHighSurrogate(c) && n+1<N) {
      // utf16 long characters
      s = txt.mid(n, 2);
      charwidths[n] = 0; // store width with second of pair
      n += 1;
    }
    if (edges.contains(n)) {
      current = edges[n];
      fm = fv.metrics(current);
    }
    if (edges.contains(n+1) || n+1>=N
	|| txt[n+1].category()==QChar::Other_Control) {
      // simple, no kerning across edges or table cells
      charwidths[n] = fm->width(s);
      if (edges.contains(n+1) && current.contains(MarkupData::Italic)
	  && !edges[n+1].contains(MarkupData::Italic))
	charwidths[n] += italicCorrection(current);
    } else {
      QChar d = txt[n+1];
      QString t(d);
      if (Unicode::isHighSurrogate(d) && n+2<N)
	t = txt.mid(n+1, 2);
      charwidths[n] = fm->width(s+t) - fm->width(t);
    }
  }
}

double TextItemDocData::italicCorrection(MarkupStyles const &sty) const {
  QFontMetricsF const *fm = fv.metrics(sty);
  return fm->width(" ")*0.4;
}
