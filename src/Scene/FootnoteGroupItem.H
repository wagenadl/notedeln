// FootnoteGroupItem.H

#ifndef FOOTNOTEGROUPITEM_H

#define FOOTNOTEGROUPITEM_H

#include <QGraphicsObject>
#include "Item.H"

class FootnoteGroupItem: public Item {
  Q_OBJECT;
public:
  FootnoteGroupItem(class BlockData *data, class PageScene *parent);
  virtual ~FootnoteGroupItem();
  virtual void makeWritable();
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
public slots:
  void restack();
signals:
  void vChanged();
private:
  BlockData *data;
};

#endif
