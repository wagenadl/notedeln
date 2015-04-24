// FontVariants.cpp

#include "FontVariants.h"



FontVariants::FontVariants() {
  fmap[MarkupData::Normal] = new QFont();
}

FontVariants::FontVariants(QFont const &base) {
  fmap[MarkupData::Normal] = new QFont(base);
}

void FontVariants::setBase(QFont const &base) {
  foreach (QFont *f, fmap)
    delete f;
  foreach (QFontMetricsF *fm, fmmap)
    delete fm;
  fmap.clear();
  fmmap.clear();
  fmap[MarkupData::Normal] = new QFont(base);
}  

FontVariants::~FontVariants() {
  foreach (QFont *f, fmap)
    delete f;
  foreach (QFontMetricsF *fm, fmmap)
    delete fm;
}

MarkupData::Styles FontVariants::simplifiedStyle(MarkupData::Styles s) {
  if (s & MarkupData::Subscript)
    s |=  MarkupData::Superscript;
  return s & (MarkupData::Italic | MarkupData::Bold | MarkupData::Italic);
}

QFont const *FontVariants::font(MarkupData::Styles s) {
  s = simplifiedStyle(s);
  if (fmap.contains(s)) {
    return fmap[s];
  } else if (s & MarkupData::Italic) {
    fmap[s] = italicVersion(font(s & ~MarkupData::Italic));
    return fmap[s];
  } else if (s & MarkupData::Bold) {
    fmap[s] = boldVersion(font(s & ~MarkupData::Bold));
    return fmap[s];
  } else if (s & MarkupData::Superscript) {
    fmap[s] = scriptVersion(font(s & ~MarkupData::Superscript));
    return fmap[s];
  }
  Q_ASSERT(0); // this shouldn't happen
}

QFontMetricsF const *FontVariants::metrics(MarkupData::Styles s) {
  s = simplifiedStyle(s);
  if (!fmmap.contains(s))
    fmmap[s] = new QFontMetricsF(*font(s));
  return fmmap[s];
}

QFont *FontVariants::italicVersion(QFont const *f) {
  QFont *f1 = new QFont(*f);
  f1->setStyle(QFont::StyleItalic);
  return f1;
}

 QFont *FontVariants::boldVersion(QFont const *f) {
  QFont *f1 = new QFont(*f);
  f1->setWeight(QFont::Bold);
  return f1;
}

QFont *FontVariants::scriptVersion(QFont const *f) {
  QFont *f1 = new QFont(*f);
  f1->setPixelSize(0.7*f->pixelSize());
  return f1;
}
