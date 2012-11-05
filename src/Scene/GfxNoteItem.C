// GfxNoteItem.C

#include "GfxNoteItem.H"
#include "GfxNoteData.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QTextDocument>

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  QGraphicsObject(Item::gi(parent)),
  Item(data, *this),
  data_(data) {
  if (data->lineLengthIsZero()) {
    line = 0;
  } else {
    line = new QGraphicsLineItem(QLineF(data->pos(), data->endPoint()), this);
    line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
		      style().real("note-line-width")));
  }
  text = new TextItem(data->text(), this);
  text->setDefaultTextColor(QColor(style().string("note-text-color")));

  connect(text, SIGNAL(abandoned()),
	  this, SLOT(abandon()), Qt::QueuedConnection);
  connect(text, SIGNAL(mousePress(QPointF, Qt::MouseButton)),
	  this, SLOT(childMousePress(QPointF, Qt::MouseButton)));
		     
  QGraphicsDropShadowEffect *s = new QGraphicsDropShadowEffect(this);
  QColor c(style().string("note-shadow-color"));
  c.setAlphaF(style().real("note-shadow-alpha"));
  s->setColor(c);
  s->setBlurRadius(style().real("note-shadow-blur"));
  s->setOffset(QPointF(0,0));
  setGraphicsEffect(s);

  setFlag(ItemIsFocusable);
  setFocusProxy(text);
  connect(text->document(), SIGNAL(contentsChanged()),
	  SLOT(updateTextPos()));
  updateTextPos();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::abandon() {
  Item *p = itemParent();
  Q_ASSERT(p);
  p->abandonNote(this);
}

void GfxNoteItem::updateTextPos() {
  QPointF p = data_->endPoint();
  double yof = style().real("note-y-offset");
  p += QPointF(0, yof);
  if (data_->dx() < 0)
    p -= QPointF(text->boundingRect().width(), 0);
  text->setPos(p);
}

QRectF GfxNoteItem::boundingRect() const {
  return QRectF();
  //  return childrenBoundingRect(); // this is not really how it's supposed to go, but otherwise, we don't seem to matter for
}

void GfxNoteItem::paint(QPainter *,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
}

void GfxNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  QPointF delta = e->pos() - e->lastPos();
  text->setPos(text->pos() + delta);
  if (line) {
    QLineF l = line->line(); // origLine;
    l.setP2(l.p2() + delta);
    if (e->modifiers() & Qt::ShiftModifier) 
      l.setP1(l.p1() + delta);
    line->setLine(l);
  }
  e->accept();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  unlockBounds();
  ungrabMouse();
  if (line) {
    QLineF l = line->line();
    QPointF p0 = l.p1();
    QPointF p1 = l.p2();
    data_->setPos(p0);
    data_->setEndPoint(p1);
  } else {
    data_->setPos(text->pos() - QPointF(0, style().real("note-y-offset")));
  }
  if (itemParent())
    itemParent()->childGeometryChanged();
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  Q_ASSERT(parent);
  GfxNoteData *d = new GfxNoteData(parent->data());
  d->setPos(p0);
  d->setEndPoint(p1);

  GfxNoteItem *i = new GfxNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}

void GfxNoteItem::childMousePress(QPointF, Qt::MouseButton b) {
  if (b==moveButton() && moveModPressed()) {
    text->setFocus();
    text->clearFocus();
    lockBounds();
    grabMouse();
  }
}

void GfxNoteItem::makeWritable() {
  text->makeWritable();
  //acceptModifierChanges();
}

