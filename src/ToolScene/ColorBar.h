// App/ColorBar.H - This file is part of NotedELN

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

// Colorbar.H

#ifndef COLORBAR_H

#define COLORBAR_H

#include "Toolbar.h"
#include "GfxMarkData.h"

class ColorBar: public Toolbar {
  Q_OBJECT;
public:
  ColorBar(class Mode *mode, bool asLine, QGraphicsItem *parent);
  virtual ~ColorBar();
public slots:
  void setMarkSize(double);
  void setLineWidth(double);
  void setShape(GfxMarkData::Shape);
  void setColor(QColor c);
protected:
  virtual void doLeftClick(QString id, Qt::KeyboardModifiers);
private:
  static QColor idToColor(QString);
  static QString colorToId(QColor);
private:
  Mode *mode;
};

#endif
