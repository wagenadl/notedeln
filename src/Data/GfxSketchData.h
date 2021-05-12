// Data/GfxSketchData.H - This file is part of NotedELN

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

// GfxSketchData.H

#ifndef GFXSKETCHDATA_H

#define GFXSKETCHDATA_H

#include "GfxPointsData.h"
#include <QColor>

class GfxSketchData: public GfxPointsData {
  Q_OBJECT;
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth);
public:
  GfxSketchData(Data *parent=0);
  virtual ~GfxSketchData();
  QColor color() const;
  double lineWidth() const;
  void setColor(QColor);
  void setLineWidth(double);
private:
  QColor col;
  double lw;
};

#endif
