// Items/LateNoteItem.H - This file is part of eln

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

// LateNoteItem.H

#ifndef LATENOTEITEM_H

#define LATENOTEITEM_H

#include "GfxNoteItem.H"
#include "LateNoteData.H"

class LateNoteItem: public GfxNoteItem {
public:
  LateNoteItem(LateNoteData *data, Item *parent=0);
  virtual ~LateNoteItem();
  DATAACCESS(LateNoteData);
  static LateNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
  virtual void setScale(qreal f);
  virtual bool excludeFromNet() const;
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  virtual QVariant itemChange(GraphicsItemChange change,
			      QVariant const &value);
private:
  void prepDateItem();
  void setDateItemPosition();
private:
  QGraphicsTextItem *dateItem;
  
};

#endif
