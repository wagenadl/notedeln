// TextItemDocData.cpp

#include "TextItemDocData.h"
#include "MarkupEdges.h"

TextItemDocData::TextItemDocData(TextData *text): text(text) {
  indent = 0;
  width = 1000; // hmm
  leftmargin = rightmargin = 0;
  lineheight = 15; // hmm
  selstart = selend = -1;
  writable = false;
}

void TextItemDocData::setBaseFont(QFont const &f) {
  baseFont = f;
  fv.setBase(f);
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
  
  if (charwidths.isEmpty()) {
    start = 0;
    end = -1;
  }

  MarkupStyles current;
  MarkupEdges edges(text->markups());
  foreach (int k, edges.keys()) 
    if (k<start)
      current = edges[k];
    else
      break;
  
  QFontMetricsF const *fm = fv.metrics(current);
  
  QString txt = text->text();
  int N = txt.size();
  charwidths.resize(N);
  if (end<0)
    end = N;
  
  for (int n=start; n<end; n++) {
    QChar c = txt[n];
    if (edges.contains(n)) {
      current = edges[n];
      fm = fv.metrics(current);
    }

    charwidths[n] = fm->width(c);
  }
}

