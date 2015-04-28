// Data/GfxBlockData.H - This file is part of eln

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

// GfxBlockData.H

#ifndef GFXBLOCKDATA_H

#define GFXBLOCKDATA_H

#include "BlockData.h"

class GfxBlockData: public BlockData {
  Q_OBJECT
  Q_PROPERTY(double xref READ xref WRITE setXref)
  Q_PROPERTY(double yref READ yref WRITE setYref)
public:
  GfxBlockData(Data *parent=0);
  virtual ~GfxBlockData();
  double xref() const;
  double yref() const;
  QPointF ref() const;
  void setXref(double);
  void setYref(double);
  void sneakilySetYref(double); // see BlockData.H for comment
  void setRef(QPointF);
  QList<class GfxData *> gfx() const;
  virtual bool isEmpty() const;
protected:
  double xref_, yref_;
};

#endif
