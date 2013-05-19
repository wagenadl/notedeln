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
};


#endif
