// Items/TOCItem.H - This file is part of eln

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

// TOCItem.H

#ifndef TOCITEM_H

#define TOCITEM_H

#include "Item.h"
#include <QGraphicsObject>
#include "TOCEntry.h"

class TOCItem: public Item {
  Q_OBJECT;
public:
  TOCItem(class TOCEntry *data, class BaseScene *parent);
  virtual ~TOCItem();
  DATAACCESS(TOCEntry);
  QRectF boundingRect() const;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
signals:
  void vboxChanged();
  void clicked(int, Qt::KeyboardModifiers); // page number
public slots:
  void entryChanged();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  void makeItems();
  void fillItems();
private:
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *titleItem;
  QGraphicsTextItem *pgnoItem;
  TOCEntry *data_;
  BaseScene *scene_;
};

#endif
