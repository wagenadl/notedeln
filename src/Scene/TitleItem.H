// TitleItem.H

#ifndef TITLEITEM_H

#define TITLEITEM_H

#include <QGraphicsObject>
#include "TextItem.H"
#include "TitleData.H"

class TitleItem: public TextItem {
  Q_OBJECT;
public:
  TitleItem(TitleData *data, Item *parent=0);
  virtual ~TitleItem();
  TitleData *data() const;
protected slots:
  virtual void docChange();
private:
  TitleData *data_;
};

#endif
