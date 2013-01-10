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

Item::Item(Data *d, Item *parent): QGraphicsObject(parent), d(d) {
  ASSERT(d);
  if (parent)
    parent->addChild(this);
  qDebug() << "Item::Item" << this << parent << d;
  brLocked = false;
  extraneous = false;
  writable = false;
  setAcceptHoverEvents(true);
}

Item::~Item() {
  qDebug() << "Item::~Item" << this;
}

Data *Item::data() {
  return d;
}

Style const &Item::style() const {
  static Style dflt("-");
  Notebook *n = d->book();
  if (n)
    return n->style();
  qDebug() << "Item" << this << "not connected to notebook";
  return dflt; // this happens during item deletion. horrible, but true.
}

QVariant Item::style(QString k) const {
  return style()[k];
}

bool Item::isWritable() const {
  return writable;
}

void Item::makeWritable() {
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
  
void Item::addChild(Item *i) {
  bool haveAlready = children_.contains(i);
  ASSERT(!haveAlready); // is this too aggressive?
  if (!haveAlready)
    children_.append(i);
}

bool Item::deleteChild(Item *i) {
  if (children_.removeOne(i)) {
    i->setParentItem(0);
    i->deleteLater();
    if (children_.isEmpty())
      emit childless();
    return true;
  } else {
    return false;
  }
}

bool Item::childless() const {
  return children_.isEmpty();
}

void Item::lockBounds() {
  brCache = netBoundingRect();
  brLocked = true;
}

void Item::unlockBounds() {
  brLocked = false;
  childGeometryChanged();
}

QRectF Item::cachedBounds() const {
  if (brLocked)
    return brCache;
  else
    return QRectF();
}

QRectF Item::netBoundingRect() const {
  if (brLocked)
    return brCache;
  QRectF bb = boundingRect();
  foreach (Item *i, children_) {
    if (!i->extraneous) {
      bb |= i->mapRectToParent(i->netBoundingRect());
    }
  }
  return bb;
}

QRectF Item::netSceneRect() const {
  return mapRectToScene(netBoundingRect());
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

Item *Item::itemParent() const {
  return dynamic_cast<Item*>(parentItem());
}

void Item::childGeometryChanged() {
  if (itemParent())
    itemParent()->childGeometryChanged();
}

GfxNoteItem *Item::newNote(QPointF p0, QPointF p1, bool late) {
  GfxNoteItem *n = late
    ? LateNoteItem::newNote(p0, p1, this)
    : GfxNoteItem::newNote(p0, p1, this);
  childGeometryChanged(); // b/c of the new note
  return n;
}

GfxNoteItem *Item::createNote(QPointF p0, bool late) {
  QPointF p1 = DragLine::drag(this, p0);
  return newNote(p0, p1, late);
}

static bool shouldGlow(Data *d) {
  return dynamic_cast<GfxData*>(d)
    || (dynamic_cast<TextData*>(d) &&
        dynamic_cast<GfxData*>(d->parent()));
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  qDebug() << "hoverEnterEvent" << this;
  if (writable && shouldGlow(d) && mode()->mode()==Mode::MoveResize) {
    QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
    eff->setColor(QColor("#00ff33"));
    eff->setOffset(QPointF(0, 0));
    eff->setBlurRadius(4);
    setGraphicsEffect(eff);
  }
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  qDebug() << "hoverLeaveEvent" << this;
  setGraphicsEffect(0);
}


