// BlockItem.H

#ifndef BLOCKITEM_H

#define BLOCKITEM_H

#include <QGraphicsObject>
#include "Item.H"

class BlockItem: public QGraphicsObject, public Item {
  Q_OBJECT;
public:
  BlockItem(class BlockData *data, Item *parent);
  virtual ~BlockItem();
  BlockData *data();
signals:
  void vboxChanged();
public slots:
  void checkVbox();
protected:
  void resetBbox(); // size change w/o emitting signals
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  BlockData *data_;
  QRectF oldBbox;
  // QList<NoteItem *> notes_;
};

#endif
