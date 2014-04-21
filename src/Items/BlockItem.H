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
#include "Item.H"
#include "BlockData.H"

class BlockItem: public Item {
  Q_OBJECT;
public:
  BlockItem(BlockData *data, Item *parent);
  virtual ~BlockItem();
  DATAACCESS(BlockData);
  virtual BlockItem const *ancestralBlock() const;
  virtual BlockItem *ancestralBlock();
  void setBaseScene(class BaseScene *);
  BaseScene *baseScene();
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
  BaseScene *bs;
};


#endif
