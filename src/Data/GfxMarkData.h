// Data/GfxMarkData.H - This file is part of NotedELN

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

// GfxMarkData.H

#ifndef GFXMARKDATA_H

#define GFXMARKDATA_H

#include "GfxData.h"
#include <QColor>

class GfxMarkData: public GfxData {
  Q_OBJECT;
  Q_ENUMS(Shape)
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double size READ size WRITE setSize);
  Q_PROPERTY(Shape shape READ shape WRITE setShape);
public:
  enum Shape {
    SolidCircle,
    SolidSquare,
    Cross,
    Plus,
    OpenCircle,
    OpenSquare,
    DotCircle,
    DotSquare,
    LAST=DotSquare,
  };
public:
  GfxMarkData(Data *parent=0);
  virtual ~GfxMarkData();
  QColor color() const;
  double size() const;
  Shape shape() const;
  void setColor(QColor);
  void setSize(double);
  void setShape(Shape);
private:
  QColor col;
  double siz;
  Shape shp;
};

#endif

