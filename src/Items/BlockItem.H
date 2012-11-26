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
  static BlockItem const *ancestralBlock(Item const *);
  static BlockItem *ancestralBlock(Item *);
signals:
  void vboxChanged();
public slots:
  void checkVbox();
  void refTextChange(QString olds, QString news);
protected:
  void resetBbox(); // size change w/o emitting signals
private:
  BlockData *data_;
  QRectF oldBbox;
  // QList<NoteItem *> notes_;
};


#endif
