// TitleItem.C

#include "TitleItem.H"
#include "TitleTextItem.H"
#include "Style.H"
#include <QDebug>
#include <QTextDocument>

TitleItem::TitleItem(TitleData *data, QGraphicsObject *parent):
  QGraphicsObject(parent), data_(data) {
  Style const &style(Style::defaultStyle());
  qDebug() << "TitleItem!";
  text_ = new TitleTextItem(data_, this);
  qDebug() << "Created TitleTextItem";

  text_->setFont(QFont(style["title-font-family"].toString(),
			      style["title-font-size"].toDouble()));
  text_->setDefaultTextColor(QColor(style["title-color"].toString()));
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
