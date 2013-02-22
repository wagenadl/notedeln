// Item.C

#include "Item.H"
#include "PageScene.H"
#include <QDebug>
#include "Notebook.H"
#include "LateNoteItem.H"
#include "PageScene.H"
#include <QGraphicsSceneMouseEvent>
#include "DragLine.H"
#include "App.H"
#include "GfxData.H"
#include "GfxNoteData.H"
#include "Assert.H"
#include <QGraphicsDropShadowEffect>
#include "Mode.H"
#include "BlockItem.H"

Item::Item(Data *d, Item *parent): QGraphicsObject(parent), d(d) {
  ASSERT(d);
  extraneous = false;
  writable = false;
  setAcceptHoverEvents(true);
}

Item::~Item() {
}

void Item::deleteLater() {
  ASSERT(d);
  d = 0;
  QGraphicsObject::setParentItem(0);
  if (scene()) 
    scene()->removeItem(this);
  QGraphicsObject::deleteLater();
}

Style const &Item::style() const {
  ASSERT(d);
  Notebook *n = d->book();
  if (n)
    return n->style();
  qDebug() << "Item" << this << "not connected to notebook";
  // This appears to happens during item deletion. horrible, but true.
  return Style::defaultStyle();
}

QVariant Item::style(QString k) const {
  return style()[k];
}

bool Item::isWritable() const {
  return writable;
}

void Item::makeWritable() {
  ASSERT(d);
  writable = true;
}

PageScene *Item::pageScene() const {
  return dynamic_cast<PageScene*>(scene());
}

Mode const *Item::mode() const {
  ASSERT(d);
  Notebook *nb = d->book();
  ASSERT(nb);
  return nb->mode();
}

Item *Item::create(Data *d, Item *parent) {
  ASSERT(d);
  if (creators().contains(d->type()))
    return creators()[d->type()](d, parent);
  qDebug() << "Item::create: No creator for " << d->type();
  ASSERT(0);
  return 0;
}

QMap<QString, Item *(*)(Data *, Item *)> &Item::creators() {
  static QMap<QString, Item *(*)(Data *, Item *)> m;
  return m;
}
  
QRectF Item::netChildBoundingRect() const {
  ASSERT(d);
  QRectF bb;
  foreach (Item *i, allChildren()) {
    if (!i->isExtraneous()) {
      QRectF b = i->boundingRect();
      b |= i->netChildBoundingRect();
      bb |= i->mapRectToParent(b);
    }
  }
  return bb;
}


Item *Item::parent() const {
  return dynamic_cast<Item*>(QGraphicsObject::parentItem());
}

QList<Item*> Item::allChildren() const {
  return children<Item>();
}

void Item::setExtraneous(bool e) {
  extraneous = e;
}

bool Item::isExtraneous() const {
  return extraneous;
}

Qt::CursorShape Item::defaultCursor() {
  return Qt::ArrowCursor;
}

GfxNoteItem *Item::newNote(QPointF p0, QPointF p1, bool late) {
  ASSERT(d);
  GfxNoteItem *n = late
    ? LateNoteItem::newNote(p0, p1, this)
    : GfxNoteItem::newNote(p0, p1, this);
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  return n;
}

GfxNoteItem *Item::createNote(QPointF p0, bool late) {
  ASSERT(d);
  QPointF p1 = DragLine::drag(this, p0);
  return newNote(p0, p1, late);
}

static bool shouldGlow(Data *d) {
  return dynamic_cast<GfxData*>(d)
    || (dynamic_cast<TextData*>(d) &&
        dynamic_cast<GfxData*>(d->parent()));
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  ASSERT(d);
  if (writable && shouldGlow(d) && mode()->mode()==Mode::MoveResize) {
    QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
    eff->setColor(QColor("#00ff33"));
    eff->setOffset(QPointF(0, 0));
    eff->setBlurRadius(4);
    setGraphicsEffect(eff);
  }
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  ASSERT(d);
  setGraphicsEffect(0);
}


BlockItem const *Item::ancestralBlock() const {
  return parent() ? parent()->ancestralBlock() : 0;
}

BlockItem *Item::ancestralBlock() {
  return parent() ? parent()->ancestralBlock() : 0;
}

void Item::setScale(qreal f) {
  QGraphicsObject::setScale(f);
}
