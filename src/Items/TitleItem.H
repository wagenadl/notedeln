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
  TitleData const *data() const { ASSERT(d); return d; }
  TitleData *data() { ASSERT(d); return d; }
  virtual void deleteLater();
protected slots:
  virtual void docChange();
private:
  virtual bool focusIn(QFocusEvent *);
  TitleData *d;
};

#endif
