// Items/GfxLineItem.H - This file is part of eln

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

// GfxLineItem.H

#ifndef GFXLINEITEM_H

#define GFXLINEITEM_H

#include "Item.h"
#include "GfxLineData.h"
#include "Mode.h"

#include <QGraphicsObject>

class GfxLineItem: public Item {
  Q_OBJECT;
public:
  GfxLineItem(GfxLineData *data, Item *parent=0);
  virtual ~GfxLineItem();
  DATAACCESS(GfxLineData);
  virtual QRectF boundingRect() const;
  virtual QPainterPath shape() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void makeWritable();
  static GfxLineItem *newLine(QPointF p,
			      QColor c, double lw,
			      Item *parent);
  static GfxLineItem *newLine(QPointF p, Item *parent);
  void build(QGraphicsSceneMouseEvent *);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  virtual bool changesCursorShape() const;
  virtual Qt::CursorShape cursorShape(Qt::KeyboardModifiers) const;
private:
  void rebuildPath();
  void moveBuilding(QGraphicsSceneMouseEvent *);
  QPointF roundToGrid(QPointF) const;
signals:
  void doneBuilding();
private:
  bool building;
  QPainterPath path;
  QPainterPath stroked;
  QPointF presspt, presspt2;
  int pressidx;
};

#endif
