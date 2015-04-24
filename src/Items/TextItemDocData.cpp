// TextItemDocData.cpp

#include "TextItemDocData.h"

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
  
  QMap<int, MarkupData::Styles> ends;
  QMap<int, MarkupData::Styles> starts;
  foreach (MarkupData *m, text->markups()) {
    if (m->end()>m->start())
      starts[m->start()] |= m->style();
    ends[m->end()] |= m->style();
  }

  MarkupData::Styles current = MarkupData::Normal;
  for (int n=0; n<start; n++) {
    if (ends.contains(n)) 
      current &= ~ends[n];
    if (starts.contains(n)) 
      current |= ends[n];
  }    

  QFontMetricsF const *fm = fv.metrics(current);
  
  QString txt = text->text();
  int N = txt.size();
  charwidths.resize(N);
  if (end<0)
    end = N;
  
  for (int n=start; n<end; n++) {
    QChar c = txt[n];
    if (ends.contains(n)) {
      current &= ~ends[n];
      fm = fv.metrics(current);
    }
    if (starts.contains(n)) {
      current |= ends[n];
      fm = fv.metrics(current);
    }

    charwidths[n] = fm->width(c);
  }
}

