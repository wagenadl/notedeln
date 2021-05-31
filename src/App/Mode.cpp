// App/Mode.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Mode.C

#include "Mode.h"
#include <QDebug>

Mode::Mode(bool ro, QObject *parent): QObject(parent), ro(ro) {
  m = Browse;
  typem = Normal;
  drawm = Freehand;
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

Mode::TypeM Mode::typeMode() const {
  return typem;
}

Mode::DrawM Mode::drawMode() const {
  return drawm;
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
    qDebug() << "Caution: setMode ignored on read-only";
    m1 = Browse;
  } else if (!writable
             && !(m1==Browse || m1==Annotate
                  || m1==Highlight || m1==Strikeout || m1==Plain)) {
    qDebug() << "Caution: setMode ignored on nonwritable";
    m1 = Browse;
  }
  m = m1;
  emit modeChanged(m);
}

void Mode::setTypeMode(Mode::TypeM ts) {
  if (ro) {
    qDebug() << "Caution: setMathMode ignored on RO";
    return;
  }
  typem = ts;
  emit typeModeChanged(typem);
}

void Mode::setDrawMode(Mode::DrawM m1) {
  if (ro) {
    qDebug() << "Caution: setStraightLineMode ignored on RO";
    return;
  }
  drawm = m1;
  emit drawModeChanged(drawm);
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

void Mode::setWritable(bool wr) {
  writable = wr;
  if (writable)
    setMode(Type);
  else
    setMode(Browse);
}
