// Items/GfxBlockItem.H - This file is part of NotedELN

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

// GfxBlockItem.H

#ifndef GFXBLOCKITEM_H

#define GFXBLOCKITEM_H

#include "BlockItem.h"
#include <Qt>
#include <QUrl>
#include "GfxBlockData.h"

class GfxBlockItem: public BlockItem {
  Q_OBJECT;
public:
  GfxBlockItem(GfxBlockData *data, Item *parent=0);
  virtual ~GfxBlockItem();
  DATAACCESS(GfxBlockData);
  Item *newVideo(QImage keyimg, double dur, QUrl src, QPointF xy);
  Item *newImage(QImage img, QUrl src, QPointF xy);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  QRectF boundingRect() const;
  virtual void makeWritable();
public:
  bool makesOwnNotes() const { return isWritable(); }
public slots:
  void sizeToFit();
public: // should be protected, but EntryScene calls it
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
protected:
  virtual bool changesCursorShape() const;
  virtual Qt::CursorShape cursorShape(Qt::KeyboardModifiers) const;
private:
  double availableWidth() const;
  QRectF generousChildrenBounds() const;
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
  void createMark(QPointF pos);
  bool perhapsSendMousePressToChild(QGraphicsSceneMouseEvent *);
};

#endif
