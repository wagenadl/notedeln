// App/Mode.H - This file is part of eln

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

// Mode.H

#ifndef MODE_H

#define MODE_H

#include <QObject>
#include "GfxMarkData.h"

class Mode: public QObject {
  Q_OBJECT;
public:
  enum M {
    Browse,
    Type,
    MoveResize,
    Mark,
    Freehand,
    Annotate,
    Highlight,
    Strikeout,
    Plain,
  };
public:
  Mode(QObject *parent=0);
  virtual ~Mode();
  M mode() const;
  double lineWidth() const;
  QColor color() const;
  GfxMarkData::Shape shape() const;
  double markSize() const;
  bool mathMode() const;
public slots:
  void setMode(Mode::M);
  void setLineWidth(double);
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
  void setMarkSize(double);
  void setMathMode(bool);
  void temporaryOverride(Mode::M);
  void temporaryRelease(Mode::M);
signals:
  void modeChanged(Mode::M);
  void lineWidthChanged(double);
  void colorChanged(QColor);
  void shapeChanged(GfxMarkData::Shape);
  void markSizeChanged(double);
  void mathModeChanged(bool);
private:
  M m;
  bool math;
  double lw;
  QColor c;
  GfxMarkData::Shape shp;
  double ms;
  M overridden;
};

#endif
