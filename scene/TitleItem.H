// TitleItem.H

#ifndef TITLEITEM_H

#define TITLEITEM_H

#include <QGraphicsObject>
#include "TextItem.H"
#include "TitleData.H"

class TitleItem: public QGraphicsObject {
  Q_OBJECT;
public:
  TitleItem(TitleData *data, QGraphicsObject *parent=0);
  virtual ~TitleItem();
  TextItem *text();
  TitleData *data() const;
  QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
private:
  class TitleTextItem *text_;
  TitleData *data_;
};

#endif
