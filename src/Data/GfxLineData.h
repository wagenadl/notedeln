// Data/GfxLineData.H - This file is part of eln

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

// GfxLineData.H

#ifndef GFXLINEDATA_H

#define GFXLINEDATA_H

#include "GfxPointsData.h"
#include <QColor>

class GfxLineData: public GfxPointsData {
  Q_OBJECT;
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth);
public:
  GfxLineData(Data *parent=0);
  virtual ~GfxLineData();
  QColor color() const;
  double lineWidth() const;
  void setColor(QColor);
  void setLineWidth(double);
private:
  QColor col;
  double lw;
};

#endif
