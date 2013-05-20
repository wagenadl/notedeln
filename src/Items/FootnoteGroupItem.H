// Items/FootnoteGroupItem.H - This file is part of eln

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

// FootnoteGroupItem.H

#ifndef FOOTNOTEGROUPITEM_H

#define FOOTNOTEGROUPITEM_H

#include <QGraphicsObject>
#include "Item.H"
#include "BlockData.H"

class FootnoteGroupItem: public Item {
  Q_OBJECT;
public:
  FootnoteGroupItem(BlockData *data, class EntryScene *parent);
  virtual ~FootnoteGroupItem();
  DATAACCESS(BlockData);
  virtual void makeWritable();
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  double netHeight() const; // sum height of all notes
  bool resetPosition(); // position all footnotes at the y0 stored in their data
  // returns true if any were at y0=0
  void moveTo(double y); // change the y0 for all footnotes is the group
  // arg. is the top of the first note. This changes the data
signals:
  void heightChanged();
  /* Emitted when the height of any of the constituent footnotes has changed.
     Receiver is responsible for repositioning footnotes
   */
public slots:
  void childChanged();
};

#endif
