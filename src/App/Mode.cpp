// App/Mode.cpp - This file is part of eln

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

// Mode.C

#include "Mode.H"

Mode::Mode(QObject *parent): QObject(parent) {
  m = Browse;
  overridden = Browse;
  lw = 2;
  ms = 10;
  shp = GfxMarkData::SolidCircle;
}

Mode::~Mode() {
}

Mode::M Mode::mode() const {
  return m;
}

double Mode::lineWidth() const {
  return lw;
}

QColor Mode::color() const {
  return c;
}

GfxMarkData::Shape Mode::shape() const {
  return shp;
}
double Mode::markSize() const {
  return ms;
}

void Mode::setMode(Mode::M m1) {
  m = m1;
  emit modeChanged(m);
}

void Mode::setLineWidth(double lw1) {
  lw = lw1;
  emit lineWidthChanged(lw);
}

void Mode::setColor(QColor c1) {
  c = c1;
  emit colorChanged(c);
}

void Mode::setShape(GfxMarkData::Shape s1) {
  shp = s1;
  emit shapeChanged(shp);
}

void Mode::setMarkSize(double ms1) {
  ms = ms1;
  emit markSizeChanged(ms);
}

void Mode::temporaryOverride(Mode::M m1) {
  overridden = m;
  setMode(m1);
}

void Mode::temporaryRelease(Mode::M m1) {
  if (m==m1)
    setMode(overridden);
}
