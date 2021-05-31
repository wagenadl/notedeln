// App/Mode.H - This file is part of NotedELN

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
    Draw,
    Annotate,
    Highlight,
    Strikeout,
    Plain,
    Invalid,
  };
  enum TypeM {
    Normal,
    Math,
    Code
  };
  enum DrawM {
    Freehand,
    Straightline
  };
public:
  Mode(bool readonly, QObject *parent=0);
  virtual ~Mode();
  M mode() const;
  double lineWidth() const;
  QColor color() const;
  GfxMarkData::Shape shape() const;
  double markSize() const;
  TypeM typeMode() const;
  DrawM drawMode() const;
  bool isReadOnly() const { return ro; } // whole notebook
  bool isWritable() const { return writable && !ro; } // this entry
public:
  static Mode *ensure(Mode *);
public slots:
  void setWritable(bool);
  void setMode(Mode::M);
  void setLineWidth(double);
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
  void setMarkSize(double);
  void setTypeMode(TypeM);
  void setDrawMode(DrawM);
signals:
  void modeChanged(Mode::M);
  void lineWidthChanged(double);
  void colorChanged(QColor);
  void shapeChanged(GfxMarkData::Shape);
  void markSizeChanged(double);
  void typeModeChanged(TypeM);
  void drawModeChanged(DrawM);
private:
  bool ro;
  bool writable;
  M m;
  TypeM typem;
  DrawM drawm;
  double lw;
  QColor c;
  GfxMarkData::Shape shp;
  double ms;
};

#endif
