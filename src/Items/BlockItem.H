// BlockItem.H

#ifndef BLOCKITEM_H

#define BLOCKITEM_H

#include <QGraphicsObject>
#include "Item.H"

class BlockItem: public Item {
  Q_OBJECT;
public:
  BlockItem(class BlockData *data, Item *parent);
  virtual ~BlockItem();
  BlockData *data();
  virtual BlockItem const *ancestralBlock() const;
  virtual BlockItem *ancestralBlock();
  QRectF boundingRect() const;
  virtual QRectF fittedRect() const;
signals:
  void boundsChanged();
public slots:
  void sizeToFit();
  void lockBounds();
  void unlockBounds();
  void refTextChange(QString olds, QString news);
private:
  BlockData *data_;
  QRectF bbox;
  bool bbLocked;
};


#endif
