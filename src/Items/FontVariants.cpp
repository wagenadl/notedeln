// Items/FontVariants.cpp - This file is part of eln

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

// FontVariants.cpp

#include "FontVariants.h"
#include "ElnAssert.h"


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
  ASSERT(0); // this shouldn't happen
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
  f1->setPixelSize(0.8*f->pixelSize());
  return f1;
}
