// App/MarkShapeBar.H - This file is part of NotedELN

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

// Markshapebar.H

#ifndef MARKSHAPEBAR_H

#define MARKSHAPEBAR_H

#include "Toolbar.h"
#include "GfxMarkData.h"

class MarkShapeBar: public Toolbar {
  Q_OBJECT;
public:
  MarkShapeBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~MarkShapeBar();
public slots:
  void setColor(QColor);
  void setMarkSize(double);
protected:
  virtual void doLeftClick(QString id, Qt::KeyboardModifiers);
private:
  static GfxMarkData::Shape idToShape(QString);
  static QString shapeToId(GfxMarkData::Shape);
private:
  Mode *mode;
};

#endif
