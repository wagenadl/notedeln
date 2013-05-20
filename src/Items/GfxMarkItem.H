// Items/GfxMarkItem.H - This file is part of eln

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

// GfxMarkItem.H

#ifndef GFXMARKITEM_H

#define GFXMARKITEM_H

#include "Item.H"
#include "GfxMarkData.H"
#include "Mode.H"

#include <QGraphicsObject>

class GfxMarkItem: public Item {
  Q_OBJECT;
public:
  GfxMarkItem(GfxMarkData *data, Item *parent=0);
  virtual ~GfxMarkItem();
  DATAACCESS(GfxMarkData);
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void makeWritable();
  static GfxMarkItem *newMark(QPointF p,
			      QColor c, double siz, GfxMarkData::Shape shp,
			      Item *parent);
  static GfxMarkItem *newMark(QPointF p, Item *parent);
  static void renderMark(QPointF p0,
			 QColor c, double siz, GfxMarkData::Shape shp,
			 QPainter *p);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void modeChange(Mode::M);
};

#endif
