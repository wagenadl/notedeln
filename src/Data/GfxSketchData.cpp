// Data/GfxSketchData.cpp - This file is part of eln

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

// GfxSketchData.C

#include "GfxSketchData.H"

static Data::Creator<GfxSketchData> c("gfxsketch");

GfxSketchData::GfxSketchData(Data *parent): GfxPointsData(parent) {
  setType("gfxsketch");
  col = QColor("black");
  lw = 1;
}

GfxSketchData::~GfxSketchData() {
}

QColor GfxSketchData::color() const {
  return col;
}

double GfxSketchData::lineWidth() const {
  return lw;
}

void GfxSketchData::setColor(QColor c) {
  col = c;
  markModified();
}

void GfxSketchData::setLineWidth(double w) {
  lw = w;
  markModified();
}

    
