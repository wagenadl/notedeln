// FootnoteGroup.C

#include "FootnoteGroupItem.H"
#include "BlockData.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "PageScene.H"

#include <QDebug>

FootnoteGroupItem::FootnoteGroupItem(BlockData *data, PageScene *parent):
  Item(data, 0) {
  parent->addItem(this);
  foreach (FootnoteData *fd, data->children<FootnoteData>()) {
    FootnoteItem *fni = new FootnoteItem(fd, this);
    connect(fni, SIGNAL(boundsChanged()), SLOT(restack()));
  }
  restack();
}

FootnoteGroupItem::~FootnoteGroupItem() {
}

void FootnoteGroupItem::restack() {
  qDebug() << "FootnoteGroupItem::restack";
  int y = 0;
  foreach (FootnoteItem *fni, children<FootnoteItem>()) {
    fni->setPos(0, y);
    fni->data()->setY0(mapToScene(QPointF(0,y)).y());
    double h = fni->netChildBoundingRect().height();
    QRectF r0 = fni->netChildBoundingRect();
    QRectF r1 = fni->boundingRect();
    qDebug() << "fngi: child y="<<y << " h="<<h;
    qDebug() << "  r0="<<r0 << " r1="<<r1;
    y += h;
  }
  qDebug() << "fngi h="<<netChildBoundingRect().height();
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

