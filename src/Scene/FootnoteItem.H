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
  void setTagText(QString);
private:
  void updateTag();
private slots:
  void abandon();
private:
  FootnoteData *data_;
  QGraphicsTextItem *tag_;
};

#endif
