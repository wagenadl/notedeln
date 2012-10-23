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
  Item(data, this),
  data_(data) {
  if (data->lineLengthIsZero()) {
    line = 0;
  } else {
    line = new QGraphicsLineItem(QLineF(data->pos(), data->endPoint()), this);
    line->setPen(QPen(QBrush(QColor(style("note-line-color").toString())),
		      style("note-line-width").toDouble()));
  }
  text = new TextItem(data->text(), this);
  text->setDefaultTextColor(QColor(style("note-text-color").toString()));
  addChild(text);

  connect(text, SIGNAL(abandoned()),
	  this, SIGNAL(abandoned()));
  connect(text, SIGNAL(mousePress(QPointF, Qt::MouseButton)),
	  this, SLOT(childMousePress(QPointF, Qt::MouseButton)));
		     
  QGraphicsDropShadowEffect *s = new QGraphicsDropShadowEffect(this);
  QColor c(style("note-shadow-color").toString());
  c.setAlphaF(style("note-shadow-alpha").toDouble());
  s->setColor(c);
  s->setBlurRadius(style("note-shadow-blur").toDouble());
  s->setOffset(QPointF(0,0));
  setGraphicsEffect(s);

  setFlag(ItemIsFocusable);
  setFocusProxy(text);
  connect(text->document(), SIGNAL(contentsChanged()),
	  SLOT(updateTextPos()));
  updateTextPos();
  acceptModifierChanges();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::updateTextPos() {
  QPointF p = data_->endPoint();
  double yof = style("note-y-offset").toDouble();
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
    data_->setPos(text->pos() - QPointF(0, style("note-y-offset").toDouble()));
  }
  if (itemParent())
    itemParent()->childGeometryChanged();
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, Item *parent) {
  return newNote(p0, p0, parent);
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  Q_ASSERT(parent);
  GfxNoteData *d = new GfxNoteData();
  d->setPos(p0);
  d->setEndPoint(p1);
  parent->data()->addChild(d);

  GfxNoteItem *i = new GfxNoteItem(d, parent);
  i->makeWritable();
  parent->addChild(i);
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
  acceptModifierChanges();
}

