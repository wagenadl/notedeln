// Items/BlockItem.H - This file is part of eln

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

// BlockItem.H

#ifndef BLOCKITEM_H

#define BLOCKITEM_H

#include <QGraphicsObject>
#include "Item.h"
#include "BlockData.h"
#include <QPointer>

class BlockItem: public Item {
  Q_OBJECT;
public:
  BlockItem(BlockData *data, Item *parent);
  virtual ~BlockItem();
  DATAACCESS(BlockData);
  virtual BlockItem const *ancestralBlock() const;
  virtual BlockItem *ancestralBlock();
  void setBaseScene(class BaseScene *);
  /* setBaseScene does *not* move the footnotes to the scene. */
  BaseScene *baseScene();
  QList<class FootnoteItem *> footnotes();
  /* Footnotes are associated with a block, but they are not children
     of a block item in the qgraphics sense except during construction. */
  virtual void makeWritable();
  class FootnoteItem *newFootnote(class FootnoteData *fnd);
  QPointF findRefText(QString);
public:
  // splitpar stuff
  virtual double visibleHeight() const;
  /* Can be the same as the block height, but could exclude bottom padding. */
  virtual double splittableY(double /*hmax*/) const { return 0; }
  virtual Item *fragment(int) { return this; }
  virtual int nFragments() const { return 1; }
  virtual void unsplit() { }
  virtual void split(QList<double>) { }
signals:
  void heightChanged();
public slots:
  virtual void sizeToFit()=0;
  /* This method will update the height of the block as stored in the Data
     to the actual height needed for the Item.
     Emits heightChanged() if the height changed.
  */
  virtual void resetPosition();
  /* This method will move the block so that the top of its (net) bounding
     box is at the position specified in the Data.
   */
  void refTextChange(QString olds, QString news);
private:
  QPointF findRefTextIn(QString, Item *);
private:
  BaseScene *bs;
  QList<QPointer<FootnoteItem> > foots;
};


#endif
