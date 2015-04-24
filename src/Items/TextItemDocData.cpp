// TextItemDocData.cpp

#include "TextItemDocData.h"

 QFont TextItemDocData::italicVersion(QFont f) {
  f.setStyle(QFont::StyleItalic);
  return f;
}

 QFont TextItemDocData::boldVersion(QFont f) {
  f.setWeight(QFont::Bold);
  return f;
}

QFont TextItemDocData::scriptVersion(QFont f) {
  f.setPixelSize(0.8*f.pixelSize());
  return f;
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
  
  QMap<MarkupData::Styles, QFontMetricsF> const &fms = metrics();
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

  QMap<MarkupData::Styles, QFontMetricsF>::const_iterator currentit
    = fms.find(simplifiedStyle(current));
  
  QString txt = text->text();
  int N = txt.size();
  charwidths.resize(N);
  if (end<0)
    end = N;
  
  for (int n=start; n<end; n++) {
    QChar c = txt[n];
    if (ends.contains(n)) {
      current &= ~ends[n];
      currentit = fms.find(simplifiedStyle(current));
    }
    if (starts.contains(n)) {
      current |= ends[n];
      currentit = fms.find(simplifiedStyle(current));
    }

    charwidths[n] = (*currentit).width(c);
  }
}

MarkupData::Styles TextItemDocData::simplifiedStyle(MarkupData::Styles s) {
  if (s & MarkupData::Subscript)
    s |=  MarkupData::Superscript;
  return s & (MarkupData::Italic | MarkupData::Bold | MarkupData::Italic);
}

QMap<MarkupData::Styles, QFontMetricsF> const &TextItemDocData::metrics() const {
  if (!mtr.isEmpty())
    return mtr;
  
  mtr.insert(MarkupData::Normal, QFontMetricsF(font));
  mtr.insert(MarkupData::Italic, QFontMetricsF(italicVersion(font)));
  mtr.insert(MarkupData::Bold, QFontMetricsF(boldVersion(font)));
  mtr.insert(MarkupData::Bold | MarkupData::Italic,
		  QFontMetricsF(italicVersion(boldVersion(font))));
  mtr.insert(MarkupData::Superscript,
		  QFontMetricsF(scriptVersion(font)));
  mtr.insert(MarkupData::Superscript | MarkupData::Italic,
		  QFontMetricsF(italicVersion(scriptVersion(font))));
  mtr.insert(MarkupData::Superscript | MarkupData::Bold,
		  QFontMetricsF(boldVersion(scriptVersion(font))));
  mtr.insert(MarkupData::Superscript | MarkupData::Bold
		  | MarkupData::Italic,
		  QFontMetricsF(italicVersion
				(boldVersion(scriptVersion(font)))));

  return mtr;
}
