// App/Toolbars.H - This file is part of eln

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

// Toolbars.H

#ifndef TOOLBARS_H

#define TOOLBARS_H

#include "Mode.h"

#include <QGraphicsObject>
#include <Qt>

class Toolbars: public QGraphicsObject {
  Q_OBJECT;
public:
  Toolbars(class Mode *mode, QGraphicsItem *parent=0);
  virtual ~Toolbars();
  QRectF boundingRect() const { return QRectF(); }
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) { }
  class Navbar *navbar() const; // use this to connect to its signals
public slots:
  void hideTools();
  void showTools();
  void setMode(Mode::M);
private:
  void placeChildren();
private:
  bool ro;
  class Toolbar *modes;
  class Toolbar *mcolors, *lcolors;
  class Toolbar *shapes;
  class Toolbar *sizes;
  class Toolbar *widths;
  class Navbar *nav;
  Mode::M m;
};

#endif
