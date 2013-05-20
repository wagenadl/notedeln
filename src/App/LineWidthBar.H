// App/LineWidthBar.H - This file is part of eln

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

// Linewidthbar.H

#ifndef LINEWIDTHBAR_H

#define LINEWIDTHBAR_H

#include "Toolbar.H"

class LineWidthBar: public Toolbar {
  Q_OBJECT;
public:
  LineWidthBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~LineWidthBar();
public slots:
  void setColor(QColor);
protected:
  virtual void doLeftClick(QString id);
private:
  static double idToWidth(QString);
  static QString widthToId(double);
private:
  Mode *mode;
};

#endif
