// FootnoteItem.H

#ifndef FOOTNOTEITEM_H

#define FOOTNOTEITEM_H

#include "TextBlockItem.H"
#include "FutileMovementInfo.H"

class FootnoteItem: public TextBlockItem {
  Q_OBJECT;
public:
  FootnoteItem(class FootnoteData *data, Item *parent);
  virtual ~FootnoteItem();
  FootnoteData *data();
  class QGraphicsTextItem *tag();
private:
  FootnoteData *data_;
  QGraphicsTextItem *tag_;
};

#endif
