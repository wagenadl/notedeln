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

  connect(text, SIGNAL(abandoned()),
	  this, SIGNAL(abandoned()));

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
  /* This apparently never gets called because we have a zero bbox.
   */     
  qDebug() << "GfxNoteItem::mousePressEvent";
  e->ignore();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  qDebug() << "GfxNoteItem::mouseReleaseEvent";
  ungrabMouse();
}

void GfxNoteItem::modifierChange(Qt::KeyboardModifiers m) {
  qDebug() << "GfxNoteItem::modifierChange" << m;
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
  parent->addChild(i);
  i->setFocus();
  return i;
}

