// TOCItem.C

#include "TOCItem.H"
#include "TOCEntry.H"
#include "TOCScene.H"
#include <QDebug>
#include <QDateTime>

TOCItem::TOCItem(TOCEntry *data, TOCScene *parent):
  QGraphicsObject(0), Item(data, this), data_(data), scene_(parent) {
  parent->addItem(this);
  makeItems();
  fillItems();
  positionItems();
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
  titleItem->setTextWidth(style().real("text-width"));
  pgnoItem->setDefaultTextColor(style().color("toc-pgno-color"));
}

void TOCItem::fillItems() {
  dateItem->setPlainText(data_->created()
			 .toString(style().string("toc-date-format")));
  titleItem->setPlainText(data_->title());
  pgnoItem->setPlainText(QString::number(data_->startPage()));
}

void TOCItem::positionItems() {
  dateItem->setPos(dateItem->pos()
		   + QPointF(0, 0) - dateItem->boundingRect().topRight());

  pgnoItem->setPos(pgnoItem->pos()
		   + QPointF(style().real("text-width"), 0)
		   - pgnoItem->boundingRect().topLeft());
  titleItem->setPos(QPointF(0, 0));
}

QRectF TOCItem::boundingRect() const {
  return QRectF();
}

TOCEntry *TOCItem::data() {
  return data_;
}

bool TOCItem::resetVBox() {
  QRectF r = childrenBoundingRect();
  positionItems();
  return r != childrenBoundingRect();
}

void TOCItem::entryChanged() {
  fillItems();
  if (resetVBox())
    emit vboxChanged();
}

void TOCItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "TOCItem::mousePress" << e;
}

void TOCItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}
