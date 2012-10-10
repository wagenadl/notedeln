// TitleItem.C

#include "TitleItem.H"
#include "TitleTextItem.H"

TitleItem::TitleItem(TitleData *data, QGraphicsObject *parent):
  QGraphicsObject(parent), data_(data) {
  text_ = new TitleTextItem(data_);
}

TitleItem::~TitleItem() {
}

TextItem *TitleItem::text() {
  return text_;
}

TitleData *TitleItem::data() const {
  return data_;
}

QRectF TitleItem::boundingRect() const {
  return text_->mapRectToParent(text_->boundingRect());
}

void TitleItem::paint(QPainter *p,
		      const QStyleOptionGraphicsItem *o,
		      QWidget *w) {
  text_->paint(p, o, w);
}
