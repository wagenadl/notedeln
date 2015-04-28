// Data/GfxMarkData.cpp - This file is part of eln

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

// GfxMarkData.C

#include "GfxMarkData.h"

static Data::Creator<GfxMarkData> c("gfxmark");

GfxMarkData::GfxMarkData(Data *parent): GfxData(parent) {
  setType("gfxmark");
  col = QColor("black");
  siz = 5;
  shp = SolidCircle;
}

GfxMarkData::~GfxMarkData() {
}

QColor GfxMarkData::color() const {
  return col;
}
    
double GfxMarkData::size() const {
  return siz;
}

GfxMarkData::Shape GfxMarkData::shape() const {
  return shp;
}

void GfxMarkData::setColor(QColor c) {
  if (col==c)
    return;
  col = c;
  markModified();
}

void GfxMarkData::setSize(double s) {
  if (siz==s)
    return;
  siz = s;
  markModified();
}

void GfxMarkData::setShape(Shape s) {
  if (shp==s)
    return;
  shp = s;
  markModified();
}

