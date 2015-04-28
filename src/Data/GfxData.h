// Data/GfxData.H - This file is part of eln

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

// GfxData.H

#ifndef GFXDATA_H

#define GFXDATA_H

#include "Data.h"
#include <QPointF>

class GfxData: public Data {
  Q_OBJECT
  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)
public:
  GfxData(Data *parent=0);
  ~GfxData();
  // read properties
  double x() const;
  double y() const;
  // write properties
  void setX(double);
  void setY(double);
  // convenience
  QPointF pos() const;
  void setPos(QPointF);
protected:
  double x_;
  double y_;
};

#endif
