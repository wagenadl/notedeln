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

#include "TextData.H"

static Data::Creator<TextData> c("text");

TextData::TextData(Data *parent):
  Data(parent) {
  setType("text");
}

TextData::~TextData() {
}

bool TextData::isEmpty() const {
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
