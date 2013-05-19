// FootnoteItem.H

#ifndef FOOTNOTEITEM_H

#define FOOTNOTEITEM_H

#include "TextBlockItem.H"
#include "FutileMovementInfo.H"
#include "FootnoteData.H"

class FootnoteItem: public TextBlockItem {
  Q_OBJECT;
public:
  FootnoteItem(FootnoteData *data, Item *parent);
  virtual ~FootnoteItem();
  DATAACCESS(FootnoteData);
  class QGraphicsTextItem *tag();
  void setTagText(QString); // updates data
  QString tagText() const;
  bool setAutoContents();
public slots:
  void sizeToFit();
private:
  void updateTag();
private slots:
  void abandon();
private:
  QGraphicsTextItem *tag_;
};

#endif
