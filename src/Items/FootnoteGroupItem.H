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
  double netHeight() const; // sum height of all notes
  void resetPosition(); // position all footnotes at the y0 stored in their data
  void moveTo(double y); // change the y0 for all footnotes is the group
  // arg. is the top of the first note. This changes the data
signals:
  void heightChanged();
  /* Emitted when the height of any of the constituent footnotes has changed.
     Receiver is responsible for repositioning footnotes
   */
};

#endif
