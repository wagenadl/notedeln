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

#include "Mode.h"
#include <QDebug>

Mode::Mode(bool ro, QObject *parent): QObject(parent), ro(ro) {
  m = Browse;
  math = false;
  straight = false;
  lw = 1.5;
  ms = 10;
  shp = GfxMarkData::SolidCircle;
}

Mode::~Mode() {
}

Mode *Mode::ensure(Mode *m) {
  static Mode *m0 = 0;
  if (m)
    return m;
  if (!m0)
    m0 = new Mode(true);
  return m0;
}

Mode::M Mode::mode() const {
  return m;
}

bool Mode::isMathMode() const {
  return math;
}

bool Mode::isStraightLineMode() const {
  return straight;
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
  if (ro && m1!=Browse) {
    qDebug() << "Caution: setMode ignored on RO";
    m1 = Browse;
  }
  m = m1;
  emit modeChanged(m);
}

void Mode::setMathMode(bool m1) {
  if (ro) {
    qDebug() << "Caution: setMathMode ignored on RO";
    return;
  }
  math = m1;
  emit mathModeChanged(math);
}

void Mode::setStraightLineMode(bool m1) {
  if (ro) {
    qDebug() << "Caution: setStraightLineMode ignored on RO";
    return;
  }
  straight = m1;
  emit straightLineModeChanged(straight);
}

void Mode::setLineWidth(double lw1) {
  if (ro) {
    qDebug() << "Caution: setLineWidth ignored on RO";
    return;
  }
  lw = lw1;
  emit lineWidthChanged(lw);
}

void Mode::setColor(QColor c1) {
  if (ro) {
    qDebug() << "Caution: setColor ignored on RO";
    return;
  }  c = c1;
  emit colorChanged(c);
}

void Mode::setShape(GfxMarkData::Shape s1) {
  if (ro) {
    qDebug() << "Caution: setShape ignored on RO";
    return;
  }
  shp = s1;
  emit shapeChanged(shp);
}

void Mode::setMarkSize(double ms1) {
  if (ro) {
    qDebug() << "Caution: setMarkSize ignored on RO";
    return;
  }
  ms = ms1;
  emit markSizeChanged(ms);
}

