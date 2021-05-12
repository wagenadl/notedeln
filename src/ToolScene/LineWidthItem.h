// App/LineWidthItem.H - This file is part of NotedELN

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

// LineWidthItem.H

#ifndef LINEWIDTHITEM_H

#define LINEWIDTHITEM_H

#include "ToolItem.h"

class LineWidthItem: public ToolItem {
  Q_OBJECT;
public:
  LineWidthItem(double lw);
  virtual ~LineWidthItem();
  virtual void paintContents(QPainter *);
public slots:
  void setColor(QColor);
  void setLineWidth(double);
  void setStraightLineMode(bool);
private:
  double lw;
  QColor c;
  bool straight;
};

#endif
