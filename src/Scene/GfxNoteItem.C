// GfxNoteItem.C

#include "GfxNoteItem.H"
#include "GfxNoteData.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  QGraphicsObject(Item::gi(parent)),
  Item(data, this),
  data_(data) {
  text = new TextItem(data->text(), this);
  line = new QGraphicsLineItem(QLineF(data->pos(), data->endPoint()), this);
  text->setPos(data->endPoint()); // this is inadequate
  }

GfxNoteItem::~GfxNoteItem() {
}

QRectF GfxNoteItem::boundingRect() const {
  return QRectF();
}

void GfxNoteItem::paint(QPainter *,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
}

void GfxNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxNoteItem::mouseMoveEvent";
  e->ignore();
}

void GfxNoteItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxNoteItem::mousePressEvent";
  e->ignore();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxNoteItem::mouseReleaseEvent";
  e->ignore();
}

void GfxNoteItem::modifierChange(Qt::KeyboardModifiers m) {
  qDebug() << "GfxNoteItem::modifierChange" << m;
}

