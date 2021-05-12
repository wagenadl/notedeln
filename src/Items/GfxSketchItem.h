// Items/GfxSketchItem.H - This file is part of NotedELN

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

// GfxSketchItem.H

#ifndef GFXSKETCHITEM_H

#define GFXSKETCHITEM_H

#include "Item.h"
#include "GfxSketchData.h"
#include "Mode.h"

#include <QGraphicsObject>

class GfxSketchItem: public Item {
  Q_OBJECT;
public:
  GfxSketchItem(GfxSketchData *data, Item *parent=0);
  virtual ~GfxSketchItem();
  DATAACCESS(GfxSketchData);
  virtual QRectF boundingRect() const;
  virtual QPainterPath shape() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void makeWritable();
  static GfxSketchItem *newSketch(QPointF p,
				  QColor c, double lw,
				  Item *parent);
  static GfxSketchItem *newSketch(QPointF p, Item *parent);
  void build();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  virtual bool changesCursorShape() const;
  virtual Qt::CursorShape cursorShape(Qt::KeyboardModifiers) const;
private:
  void rebuildPath();
  void moveBuilding(QGraphicsSceneMouseEvent *);
signals:
  void doneBuilding();
private:
  bool building;
  QList<QPointF> droppedPoints; // used during building
  QPainterPath path;
  QPainterPath stroked;
};

#endif
