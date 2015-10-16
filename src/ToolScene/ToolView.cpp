// App/ToolView.cpp - This file is part of eln

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

// ToolView.cpp

#include "ToolView.h"
#include "ToolScene.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsItem>
#include <QDragEnterEvent>

ToolView::ToolView(Mode *mode, QWidget *parent): QGraphicsView(parent) {
  tools = new ToolScene(mode, this);
  setStyleSheet("background: transparent");
  setFrameShape(NoFrame);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setDragMode(NoDrag);
  setFocusPolicy(Qt::NoFocus);
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  
  setScene(tools);
  connect(tools, SIGNAL(changed(const QList<QRectF> &)),
	  SLOT(autoMask()));

  setAcceptDrops(true);
}

ToolView::~ToolView() {
  // scene is deleted automatically because it has us as a QObject parent
}

Toolbars *ToolView::toolbars() {
  return tools->toolbars();
}
		   
void ToolView::setScale(double x) {
  resetTransform();
  scale(x, x);
  autoMask();
}

void ToolView::autoMask() {
  QRegion reg;
  foreach (QGraphicsItem *i, tools->items()) {
    reg |= mapFromScene(i->sceneBoundingRect().adjusted(-2,-2,4,4));
  }
  setMask(reg);
}

void ToolView::mousePressEvent(QMouseEvent *e) {
  QPointF x = mapToScene(e->pos());
  if (!tools->itemAt(x, QTransform()))
    e->ignore();
  QGraphicsView::mousePressEvent(e);
}

void ToolView::dragEnterEvent(QDragEnterEvent *e) {
  e->acceptProposedAction();
}  

void ToolView::dragMoveEvent(QDragMoveEvent *e) {
  e->acceptProposedAction();
}  

void ToolView::dropEvent(QDropEvent *e) {
  emit drop(*e);
  e->acceptProposedAction();
}  

void ToolView::setFullScreen(bool yes) {
  tools->showClock(yes);
  tools->moveClock(QRectF(mapToScene(QPoint(0,0)),
			  mapToScene(QPoint(width(), height()))));
}

void ToolView::resizeEvent(QResizeEvent *) {
  tools->moveClock(QRectF(mapToScene(QPoint(0,0)),
			  mapToScene(QPoint(width(), height()))));
}  
