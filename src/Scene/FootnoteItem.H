// FootnoteItem.H

#ifndef FOOTNOTEITEM_H

#define FOOTNOTEITEM_H

#include <QGraphicsItem>
#include "Item.H"

class FootnoteItem: public QGraphicsItem, public Item {
  /* In future, might inherit QGraphicsObject instead of QGraphicsItem */
public:
  FootnoteItem(class FootnoteData *data, Item *parent);
  ~FootnoteItem();
  FootnoteData *data();
  class QGraphicsTextItem *tagItem();
  class TextItem *defItem();
  virtual void makeWritable();
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
private:
  FootnoteData *data_;
  QGraphicsTextItem *tag;
  TextItem *def;
};

#endif
