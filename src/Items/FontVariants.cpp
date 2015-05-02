// FontVariants.cpp

#include "FontVariants.h"



FontVariants::FontVariants() {
  fmap[MarkupStyles()] = new QFont();
}

FontVariants::FontVariants(QFont const &base) {
  fmap[MarkupStyles()] = new QFont(base);
}

void FontVariants::setBase(QFont const &base) {
  foreach (QFont *f, fmap)
    delete f;
  foreach (QFontMetricsF *fm, fmmap)
    delete fm;
  fmap.clear();
  fmmap.clear();
  fmap[MarkupStyles()] = new QFont(base);
}  

FontVariants::~FontVariants() {
  foreach (QFont *f, fmap)
    delete f;
  foreach (QFontMetricsF *fm, fmmap)
    delete fm;
}

QFont const *FontVariants::font(MarkupStyles s) {
  s = s.simplified();
  if (fmap.contains(s)) {
    return fmap[s];
  } else if (s.contains(MarkupData::Italic)) {
    fmap[s] = italicVersion(font(s.without(MarkupData::Italic)));
    return fmap[s];
  } else if (s.contains(MarkupData::Bold)) {
    fmap[s] = boldVersion(font(s.without(MarkupData::Bold)));
    return fmap[s];
  } else if (s.contains(MarkupData::Superscript)) {
    fmap[s] = scriptVersion(font(s.without(MarkupData::Superscript)));
    return fmap[s];
  }
  Q_ASSERT(0); // this shouldn't happen
  static QFont foo;
  return &foo;
}

QFontMetricsF const *FontVariants::metrics(MarkupStyles s) {
  s = s.simplified();
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
