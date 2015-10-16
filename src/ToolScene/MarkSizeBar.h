// App/MarkSizeBar.H - This file is part of eln

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

// Marksizebar.H

#ifndef MARKSIZEBAR_H

#define MARKSIZEBAR_H

#include "Toolbar.h"
#include "GfxMarkData.h"

class MarkSizeBar: public Toolbar {
  Q_OBJECT;
public:
  MarkSizeBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~MarkSizeBar();
public slots:
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
protected:
  virtual void doLeftClick(QString id, Qt::KeyboardModifiers);
private:
  static double idToSize(QString);
  static QString sizeToId(double);
private:
  Mode *mode;
};

#endif
