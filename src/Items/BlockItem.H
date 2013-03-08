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
  QRectF boundingRect() const;
signals:
  void boundsChanged();
public slots:
  void sizeToFit();
  void lockBounds();
  void unlockBounds();
  void refTextChange(QString olds, QString news);
private:
  QRectF bbox;
  bool bbLocked;
};


#endif
