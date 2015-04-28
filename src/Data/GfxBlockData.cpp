// Data/GfxBlockData.cpp - This file is part of eln

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

// GfxBlockData.C

#include "GfxBlockData.h"
#include "GfxData.h"
#include <QDebug>

static Data::Creator<GfxBlockData> c("gfxblock");

GfxBlockData::GfxBlockData(Data *parent):
  BlockData(parent) {
  setType("gfxblock");
  xref_ = yref_ = 0;
}

GfxBlockData::~GfxBlockData() {
}

QList<class GfxData *> GfxBlockData::gfx() const{
  return children<GfxData>();
}

double GfxBlockData::xref() const {
  return xref_;
}

double GfxBlockData::yref() const {
  return yref_;
}

QPointF GfxBlockData::ref() const {
  return QPointF(xref_, yref_);
}

void GfxBlockData::setXref(double x) {
  if (xref_==x)
    return;
  xref_ = x;
  markModified();
}


void GfxBlockData::setYref(double y) {
  if (yref_==y)
    return;
  yref_ = y;
  markModified();
}

void GfxBlockData::sneakilySetYref(double y) {
  yref_ = y;
}


void GfxBlockData::setRef(QPointF xy) {
  if (xref_==xy.x() && yref_==xy.y())
    return;
  xref_ = xy.x();
  yref_ = xy.y();
  markModified();
}


bool GfxBlockData::isEmpty() const {
  return BlockData::isEmpty() && gfx().isEmpty();
}
