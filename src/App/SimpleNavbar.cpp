// App/SimpleNavbar.cpp - This file is part of eln

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

// SimpleNavbar.C

#include "SimpleNavbar.H"
#include "ToolItem.H"
#include "Assert.H"

#include <QGraphicsScene>

#define NAV_PREV "prev"
#define NAV_NEXT "next"

SimpleNavbar::SimpleNavbar(QGraphicsScene *scene): Toolbar(0) {
  //  setOrientation(Qt::Horizontal);
  disableSelect();
  
  ToolItem *t = new ToolItem();
  t->setSvg(":icons/nav-prev.svg");
  addTool(NAV_PREV, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-next.svg");
  addTool(NAV_NEXT, t);

  ASSERT(scene);
  scene->addItem(this);
  setPos(0,
	 scene->sceneRect().height() - childrenBoundingRect().height() - 75);
  setZValue(-1);
}


SimpleNavbar::~SimpleNavbar() {
}

void SimpleNavbar::doLeftClick(QString s) {
  if (s==NAV_PREV)
    emit goRelative(-1);
  else if (s==NAV_NEXT)
    emit goRelative(1);
}
