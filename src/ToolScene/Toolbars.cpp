// App/Toolbars.cpp - This file is part of NotedELN

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

// Toolbars.C

#include "Toolbars.h"
#include "Modebar.h"
#include "LineWidthBar.h"
#include "MarkSizeBar.h"
#include "MarkShapeBar.h"
#include "ColorBar.h"
#include "Navbar.h"

#include <QDebug>

Toolbars::Toolbars(Mode *mode, QGraphicsItem *parent): QGraphicsObject(parent) {
  m = Mode::Browse;
  modes = new Modebar(mode, this);
  mcolors = new ColorBar(mode, false, this);
  lcolors = new ColorBar(mode, true, this);
  shapes = new MarkShapeBar(mode, this);
  sizes = new MarkSizeBar(mode, this);
  widths = new LineWidthBar(mode, this);
  nav = new Navbar(this);

  connect(mode, &Mode::markSizeChanged, mcolors, &ColorBar::setMarkSize);
  connect(mode, &Mode::markSizeChanged, shapes, &MarkShapeBar::setMarkSize);

  connect(mode, &Mode::shapeChanged, mcolors, &ColorBar::setShape);
  connect(mode, &Mode::shapeChanged, sizes, &MarkSizeBar::setShape);

  connect(mode, &Mode::colorChanged, lcolors, &ColorBar::setColor);
  connect(mode, &Mode::colorChanged, mcolors, &ColorBar::setColor);
  connect(mode, &Mode::colorChanged, shapes, &MarkShapeBar::setColor);
  connect(mode, &Mode::colorChanged, sizes, &MarkSizeBar::setColor);
  connect(mode, &Mode::colorChanged, widths, &LineWidthBar::setColor);

  ro = mode->isReadOnly();
  if (ro) {
    hideTools();
    nav->hidePlus();
  } else {
    setMode(mode->mode());
  }

  placeChildren();
  
  connect(mode, &Mode::modeChanged, this, &Toolbars::setMode);
}

Toolbars::~Toolbars() {
}

void Toolbars::placeChildren() {
  modes->setPos(0, 36);

  lcolors->setPos(modes->pos()
		 + QPointF(0, modes->childrenBoundingRect().height()+10));
  mcolors->setPos(modes->pos()
		 + QPointF(0, modes->childrenBoundingRect().height()+10));
  
  sizes->setPos(mcolors->pos()
		+ QPointF(36, mcolors->childrenBoundingRect().height()
			  - sizes->childrenBoundingRect().height()));
  shapes->setPos(sizes->pos()
		- QPointF(0, shapes->childrenBoundingRect().height()+10));

  widths->setPos(lcolors->pos() + QPointF(36, 0));

  nav->setPos(mcolors->pos()
	      + QPointF(0,
			mcolors->childrenBoundingRect().height()
			- nav->childrenBoundingRect().height()));
}

Navbar *Toolbars::navbar() const {
  return nav;
}

void Toolbars::hideTools() {
  shapes->hide();
  sizes->hide();
  mcolors->hide();
  widths->hide();
  lcolors->hide();
  modes->hide();
}

void Toolbars::showTools() {
  if (ro)
    return;
  
  modes->show();
  setMode(m);
}

void Toolbars::setMode(Mode::M m1) {
  m = m1;
  if (m==Mode::Mark) {
    shapes->show();
    sizes->show();
    mcolors->show();
  } else {
    shapes->hide();
    sizes->hide();
    mcolors->hide();
  }

  if (m==Mode::Draw) {
    widths->show();
    lcolors->show();
  } else {
    widths->hide();
    lcolors->hide();
  }

  if (m==Mode::Draw || m==Mode::Mark) {
    nav->hide();
  } else {
    nav->show();
  }
}
      
