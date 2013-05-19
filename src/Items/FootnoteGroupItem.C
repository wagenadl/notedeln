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
    if (fd->height()==0) 
      fni->sizeToFit();
    fni->resetPosition();
  }
}

FootnoteGroupItem::~FootnoteGroupItem() {
}

void FootnoteGroupItem::makeWritable() {
  foreach (Item *c, children<FootnoteItem>())
    c->makeWritable();
}

double FootnoteGroupItem::netHeight() const {
  double h = 0;
  foreach (FootnoteData *fd, data()->children<FootnoteData>()) 
    h += fd->height();
  return h;
}  

bool FootnoteGroupItem::resetPosition() {
  bool anyzeros = false;
  foreach (FootnoteItem *c, children<FootnoteItem>()) {
    if (c->data()->y0()==0)
      anyzeros = true;
    c->resetPosition();
  }
  return anyzeros;
}  


QRectF FootnoteGroupItem::boundingRect() const {
  return QRectF();
}

void FootnoteGroupItem::paint(QPainter *, const QStyleOptionGraphicsItem *,
			 QWidget *) {
}

void FootnoteGroupItem::moveTo(double y) {
  foreach (FootnoteItem *c, children<FootnoteItem>()) {
    if (c->data()->y0() != y) {
      if (isWritable())
        c->data()->setY0(y);
      else
        c->data()->sneakilySetY0(y);
    }
    y += c->data()->height();
  }
  resetPosition();
}

void FootnoteGroupItem::childChanged() {
  emit heightChanged();
}
