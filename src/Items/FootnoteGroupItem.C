// FootnoteGroup.C

#include "FootnoteGroupItem.H"
#include "BlockData.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "PageScene.H"

#include <QDebug>

FootnoteGroupItem::FootnoteGroupItem(BlockData *data, PageScene *parent):
  Item(data, 0), data(data) {
  parent->addItem(this);
  foreach (FootnoteData *fd, data->children<FootnoteData>()) {
    FootnoteItem *fni = new FootnoteItem(fd, this);
    connect(fni, SIGNAL(vboxChanged()), SLOT(restack()));
  }
  restack();
}

FootnoteGroupItem::~FootnoteGroupItem() {
}

void FootnoteGroupItem::restack() {
  // qDebug() << "FootnoteGroupItem::restack";
  int y = 0;
  foreach (FootnoteItem *fni, children<FootnoteItem>()) {
    fni->setPos(0, y);
    fni->data()->setY0(mapToScene(QPointF(0,y)).y());
    y += fni->childrenBoundingRect().height();
  }
  emit vChanged();
}

void FootnoteGroupItem::makeWritable() {
  foreach (Item *c, children<FootnoteItem>())
    c->makeWritable();
}

QRectF FootnoteGroupItem::boundingRect() const {
  return QRectF();
}

void FootnoteGroupItem::paint(QPainter *, const QStyleOptionGraphicsItem *,
			 QWidget *) {
}

