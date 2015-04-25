// Data/TextData.cpp - This file is part of eln

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

// TextData.C

#include "TextData.h"
#include <QDebug>

static Data::Creator<TextData> c("text");

TextData::TextData(Data *parent):
  Data(parent) {
  setType("text");
}

TextData::~TextData() {
}

bool TextData::isEmpty() const {
  qDebug() << "TextData:isempty" << text_.isEmpty() << markups().isEmpty();
  return text_.isEmpty() && markups().isEmpty();
}

QString TextData::text() const {
  return text_;
}

QList<MarkupData *> TextData::markups() const {
  return children<MarkupData>();
}

void TextData::setText(QString const &t) {
  if (text_==t)
    return;
  text_ = t;
  markModified();
}

MarkupData *TextData::addMarkup(int start, int end,
				     MarkupData::Style style) {
  MarkupData *md = new MarkupData(start, end, style);
  addChild(md, InternalMod);
  return md;
}

void TextData::addMarkup(MarkupData *md) {
  addChild(md, InternalMod);
} 

void TextData::deleteMarkup(MarkupData *md) {
  deleteChild(md, InternalMod);
}

int TextData::offsetOfFootnoteTag(QString s) const {
  int l = s.length();
  foreach (MarkupData *md, markups()) 
    if (md->style()==MarkupData::FootnoteRef
        && md->end()-md->start()==l
        && text_.mid(md->start(), l)==s)
      return md->start();
  return -1;
}

void TextData::loadMore(QVariantMap const &src) {
  Data::loadMore(src);
  linestarts.clear();
  if (src.contains("lines"))
    foreach (QVariant v, src["lines"].toList())
      linestarts.append(v.toInt());  
}

void TextData::saveMore(QVariantMap &dst) const {
  Data::saveMore(dst);
  QVariantList xl;
  foreach (int i, linestarts)
    xl.append(QVariant(i));
  dst["lines"] = QVariant(xl);
}

QVector<int> const &TextData::lineStarts() const {
  return linestarts;
}

void TextData::setLineStart(int i, int s) {
  while (linestarts.size()<i)
    linestarts<<0;
  linestarts[i] = s;
  markModified(InternalMod);
}

void TextData::setLineStarts(QVector<int> const &s) {
  linestarts = s;
  markModified(InternalMod);
}

QList<int> TextData::paragraphStarts() const {
  QList<int> res;
  int off = 0;
  while (off>=0) {
    res << off;
    off = text_.indexOf("\n", off+1);
    if (off>=0)
      off++;
  }
  return res;
}
