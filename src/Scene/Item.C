// Item.C

#include "Item.H"
#include "PageScene.H"
#include <QDebug>

Item::Item(Data *d, QGraphicsItem *me): d(d), me(me) {
  Q_ASSERT(me);
  brLocked = false;
  extraneous = false;
}

Item::~Item() {
}

Data *Item::data() {
  return d;
}

PageScene *Item::pageScene() const {
  return me ? dynamic_cast<PageScene*>(me->scene()) : 0;
}

Item *Item::create(Data *d, Item *parent) {
  Q_ASSERT(d);
  if (creators().contains(d->type()))
    return creators()[d->type()](d, parent);
  qDebug() << "Item::create: No creator for " << d->type();
  Q_ASSERT(0);
  return 0;
}

QMap<QString, Item *(*)(Data *, Item *)> &Item::creators() {
  static QMap<QString, Item *(*)(Data *, Item *)> m;
  return m;
}
  

QGraphicsItem *Item::gi(Item *i) {
  return i ? i->me : 0;
}
  
QObject *Item::obj(Item *i) {
  return i ? dynamic_cast<QObject*>(i->me) : 0;
}


QList<Item *> const &Item::allChildren() const {
  return children_;
}

void Item::addChild(Item *i) {
  Q_ASSERT(i->me != me); // quick check that child wasn't accidentally
  // constructed like Item(data, parent) rather than Item(data, this).
  children_.append(i);
}

bool Item::deleteChild(Item *i) {
  if (children_.removeOne(i)) {
    delete i;
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
}

QRectF Item::cachedBounds() const {
  if (brLocked)
    return brCache;
  else
    return QRectF();
}

QRectF Item::netBoundingRect() const {
  // qDebug() << "Item::netBoundingRect" << this;
  if (brLocked)
    return brCache;
  QRectF bb = me->boundingRect();
  foreach (Item *i, children_) {
    if (!i->extraneous) {
      // qDebug() << "  Item" << this << ": including child " <<i;
      bb |= i->me->mapRectToParent(i->netBoundingRect());
    }
  }
  return bb;
}

QRectF Item::netSceneRect() const {
  return me->mapRectToScene(netBoundingRect());
}

void Item::setExtraneous(bool e) {
  extraneous = e;
}

bool Item::isExtraneous() const {
  return extraneous;
}

Qt::CursorShape Item::defaultCursor() {
  return Qt::CrossCursor;
  // perhaps this should be different for read-only state?
}

Qt::KeyboardModifiers Item::moveModifiers() {
  return Qt::AltModifier | Qt::MetaModifier | Qt::GroupSwitchModifier;
}

Qt::MouseButton Item::moveButton() {
  return Qt::LeftButton;
}

Item *Item::itemParent() const {
  return dynamic_cast<Item*>(me->parentItem());
}

void Item::childGeometryChanged() {
  if (itemParent())
    itemParent()->childGeometryChanged();
}

QVariant Item::style(QString k) {
  // qDebug() << "Item::style" << k;
  return Style::defaultStyle()[k];
}
