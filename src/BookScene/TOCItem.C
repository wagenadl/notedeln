// TOCItem.C

#include "TOCItem.H"
#include "TOCEntry.H"
#include "TOCScene.H"
#include <QDebug>
#include <QDateTime>

TOCItem::TOCItem(TOCEntry *data, TOCScene *parent):
  QGraphicsObject(0), Item(data, *this), data_(data), scene_(parent) {
  parent->addItem(this);
  makeItems();
  fillItems();
  connect(data, SIGNAL(mod()), this, SLOT(entryChanged()));
}

TOCItem::~TOCItem() {
}

void TOCItem::makeItems() {
  dateItem = new QGraphicsTextItem(this);
  titleItem = new QGraphicsTextItem(this);
  pgnoItem = new QGraphicsTextItem(this);

  dateItem->setFont(QFont(style().string("toc-font-family"),
			  style().real("toc-font-size")));
  titleItem->setFont(QFont(style().string("toc-font-family"),
			   style().real("toc-font-size")));
  pgnoItem->setFont(QFont(style().string("toc-font-family"),
			  style().real("toc-font-size")));

  dateItem->setDefaultTextColor(style().color("toc-date-color"));
  titleItem->setDefaultTextColor(style().color("toc-title-color"));
  pgnoItem->setDefaultTextColor(style().color("toc-pgno-color"));

  titleItem->setPos(QPointF(style().real("margin-left"), 0));
  titleItem->setTextWidth(style().real("page-width")
			  - style().real("margin-left")
			  - style().real("margin-right"));
}

void TOCItem::fillItems() {
  dateItem->setPlainText(data_->created()
			 .toString(style().string("toc-date-format")));
  titleItem->setPlainText(data_->title());
  pgnoItem->setPlainText(QString::number(data_->startPage()));
  
  dateItem->setPos(QPointF(style().real("margin-left")
			   - dateItem->boundingRect().width(), 0));
  pgnoItem->setPos(QPointF(style().real("page-width")
			   - style().real("margin-right-over")
			   - pgnoItem->boundingRect().width(), 
			   titleItem->boundingRect().height()
			   - pgnoItem->boundingRect().height()));
}

QRectF TOCItem::boundingRect() const {
  return QRectF();
}

TOCEntry *TOCItem::data() {
  return data_;
}

void TOCItem::entryChanged() {
  QRectF r = childrenBoundingRect();
  fillItems();
  if (r != childrenBoundingRect())
    emit vboxChanged();
}

void TOCItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "TOCItem::mousePress" << e;
}

void TOCItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}
