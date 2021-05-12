// App/MarkSizeItem.H - This file is part of NotedELN

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

// MarkSizeItem.H

#ifndef MARKSIZEITEM_H

#define MARKSIZEITEM_H

#include "ToolItem.h"
#include "GfxMarkData.h"

class MarkSizeItem: public ToolItem {
  Q_OBJECT;
public:
  MarkSizeItem(double ms);
  virtual ~MarkSizeItem();
  virtual void paintContents(QPainter *);
public slots:
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
  void setMarkSize(double ms);
private:
  double ms;
  QColor c;
  GfxMarkData::Shape shp;
};

#endif
