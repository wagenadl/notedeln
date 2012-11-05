// Item.C

#include "Item.H"
#include "PageScene.H"
#include <QDebug>
#include "Notebook.H"
#include "LateNoteItem.H"
#include "PageScene.H"
#include <QGraphicsSceneMouseEvent>
#include "DragLine.H"
#include "ModSnooper.H"
#include "App.H"
#include "GfxData.H"
#include "GfxNoteData.H"

Item::Item(Data *d, QGraphicsItem &me): d(d), me(&me) {
  Q_ASSERT(d);
  Item *p = itemParent();
  if (p)
    p->addChild(this);
  brLocked = false;
  extraneous = false;
  writable = false;
}

Item::~Item() {
}

Data *Item::data() {
  return d;
}

Style const &Item::style() const {
  Notebook *n = d->book();
  Q_ASSERT(n);
  return n->style();
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
  return dynamic_cast<PageScene*>(me->scene());
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

QGraphicsItem *Item::gi() {
  return me;
}

QObject *Item::obj(Item *i) {
  return i ? dynamic_cast<QObject*>(i->me) : 0;
}

QObject *Item::obj() {
  return dynamic_cast<QObject*>(me);
}


void Item::addChild(Item *i) {
  Q_ASSERT(i->me != me); // quick check that child wasn't accidentally
  // constructed like Item(data, parent) rather than Item(data, this).
  bool haveAlready = children_.contains(i);
  Q_ASSERT(!haveAlready); // is this too aggressive?
  if (!haveAlready)
    children_.append(i);
}

bool Item::deleteChild(Item *i) {
  if (children_.removeOne(i)) {
    QObject *o = obj(i);
    if (o)
      o->deleteLater();
    else
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
  if (brLocked)
    return brCache;
  QRectF bb = me->boundingRect();
  foreach (Item *i, children_) {
    if (!i->extraneous) {
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
  return Qt::ArrowCursor;
}

Qt::KeyboardModifiers Item::moveModifiers() {
  return Qt::AltModifier;
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

ModSnooper *Item::modSnooper() {
  App *app = App::instance();
  Q_ASSERT(app);
  ModSnooper *ms = app->modSnooper();
  Q_ASSERT(ms);
  return ms;
}

bool Item::moveModPressed() const {
  return (modSnooper()->keyboardModifiers() & moveModifiers()) != 0;
}

void Item::acceptModifierChanges() {
  QObject *o = obj();
  if (o) 
    o->connect(modSnooper(), SIGNAL(modifiersChanged(Qt::KeyboardModifiers)),
	       o, SLOT(modifierChange(Qt::KeyboardModifiers)));
  else
    qDebug() << "Item: not an object -> keyboard modifiers will be ignored";
}
    
GfxNoteItem *Item::newNote(QPointF p0, QPointF p1, bool late) {
  GfxNoteItem *n = late
    ? LateNoteItem::newNote(p0, p1, this)
    : GfxNoteItem::newNote(p0, p1, this);
  childGeometryChanged(); // b/c of the new note
  return n;
}

bool Item::abandonNote(GfxNoteItem *gni) {
  Q_ASSERT(gni);
  GfxNoteData *gnd = gni->data();
  if (deleteChild(gni)) {
    d->deleteChild(gnd);
    childGeometryChanged();
    return true;
  } else {
    return false;
  }
}

GfxNoteItem *Item::createNote(QPointF p0, bool late) {
  QPointF p1 = DragLine::drag(this, p0);
  return newNote(p0, p1, late);
}
