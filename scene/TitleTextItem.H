// TitleTextItem.H

#ifndef TITLETEXTITEM_H

#define TITLETEXTITEM_H

#include "TextItem.H"

class TitleTextItem: public TextItem {
  Q_OBJECT;
public:
  TitleTextItem(class TitleData *data, QGraphicsItem *parent=0);
  virtual ~TitleTextItem();
  virtual bool makeWritable();
private:
  class TitleData *titleData_;
};

#endif
