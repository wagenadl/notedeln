// Data/GfxNoteData.cpp - This file is part of eln

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

// GfxNoteData.C

#include "GfxNoteData.H"
#include "Assert.H"
#include <math.h>

static Data::Creator<GfxNoteData> c("gfxnote");

GfxNoteData::GfxNoteData(Data *parent): GfxData(parent) {
  setType("gfxnote");
  text_ = new TextData(this);
  sheet_ = 0;
  dx_ = dy_ = 0;
  textWidth_ = 0;
}

GfxNoteData::~GfxNoteData() {
}

double GfxNoteData::dx() const {
  return dx_;
}

double GfxNoteData::dy() const {
  return dy_;
}

double GfxNoteData::textWidth() const {
  return textWidth_;
}

int GfxNoteData::sheet() const {
  return sheet_;
}

void GfxNoteData::setDx(double d) {
  dx_ = d;
  markModified();
}

void GfxNoteData::setDy(double d) {
  dy_ = d;
  markModified();
}

void GfxNoteData::setTextWidth(double d) {
  textWidth_ = d;
  markModified();
}

void GfxNoteData::setSheet(int s) {
  sheet_ = s;
  markModified();
}

QPointF GfxNoteData::delta() const {
  return QPointF(dx(), dy());
}

void GfxNoteData::setDelta(QPointF p) {
  dx_ = p.x();
  dy_ = p.y();
  markModified();
}

TextData const *GfxNoteData::text() const {
  return text_;
}

TextData *GfxNoteData::text() {
  return text_;
}

void GfxNoteData::loadMore(QVariantMap const &src) {
  GfxData::loadMore(src);
  text_ = firstChild<TextData>();
  ASSERT(text_);
}
