// FootnoteGroupItem.H

#ifndef FOOTNOTEGROUPITEM_H

#define FOOTNOTEGROUPITEM_H

#include <QGraphicsObject>
#include "Item.H"
#include "BlockData.H"

class FootnoteGroupItem: public Item {
  Q_OBJECT;
public:
  FootnoteGroupItem(BlockData *data, class PageScene *parent);
  virtual ~FootnoteGroupItem();
  DATAACCESS(BlockData);
  virtual void makeWritable();
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
public slots:
  void restack();
signals:
  void vChanged();
};

#endif
