// FootnoteGroupItem.H

#ifndef FOOTNOTEGROUPITEM_H

#define FOOTNOTEGROUPITEM_H

#include <QGraphicsObject>
#include "Item.H"

class FootnoteGroupItem: public QGraphicsObject, public Item {
  Q_OBJECT;
public:
  FootnoteGroupItem(class BlockData *data, class PageScene *parent);
  virtual ~FootnoteGroupItem();
  virtual void makeWritable();
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
private:
  BlockData *data;
};

#endif
