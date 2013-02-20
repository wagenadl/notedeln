// GfxNoteItem.C

#include "GfxNoteItem.H"
#include "GfxNoteData.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include "Assert.H"
#include "BlockItem.H"

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  if (data->lineLengthIsZero()) {
    line = 0;
  } else {
    line = new QGraphicsLineItem(QLineF(QPointF(0,0), data->delta()), this);
    line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
		      style().real("note-line-width")));
  }
  text = new TextItem(data->text(), this);
  text->setDefaultTextColor(QColor(style().string("note-text-color")));
  if (data->textWidth()>1)
    text->setTextWidth(data->textWidth());

  connect(text, SIGNAL(abandoned()),
	  this, SLOT(abandon()), Qt::QueuedConnection);
		     
  setFlag(ItemIsFocusable);
  setFocusProxy(text);
  connect(text->document(), SIGNAL(contentsChanged()),
	  SLOT(updateTextPos()));
  updateTextPos();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::abandon() {
  BlockItem *ancestor = ancestralBlock();
  Item *p = parent();
  if (p)
    p->data()->deleteChild(data());
  deleteLater();
  if (ancestor)
    ancestor->sizeToFit();
}

void GfxNoteItem::updateTextPos() {
  QPointF p = data()->delta();
  double yof = style().real("note-y-offset");
  p += QPointF(0, yof);
  QRectF sr = text->mapRectToScene(text->fittedRect());
  if (data()->dx() < 0)
    p -= QPointF(sr.width(), 0);
  text->setPos(p);

  if (data()->textWidth()<1) {
    if (data()->dx()>=0) {
      if (sr.right() >= style().real("page-width")
          - style().real("margin-right-over")) {
        double tw = style().real("page-width")
          - style().real("margin-right-over")
          - sr.left();
        data()->setTextWidth(tw);
        text->setTextWidth(tw);
      }
    } else {
      if (sr.left() <= style().real("margin-left")/2) {
        double tw = sr.right()
          - style().real("margin-left")/2;
        data()->setTextWidth(tw);
        text->setTextWidth(tw);
      }
    }
  }        
}

QRectF GfxNoteItem::boundingRect() const {
  return QRectF();
}

void GfxNoteItem::paint(QPainter *,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
}

void GfxNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  if (resizing) {
    QPointF delta = e->pos() - e->buttonDownPos(Qt::LeftButton);
    double w = initialTextWidth + delta.x();
    if (w<30)
      w = 30;
    text->setTextWidth(w);
  } else {
    QPointF delta = e->pos() - e->lastPos();
    text->setPos(text->pos() + delta);
    if (line) {
      QLineF l = line->line(); // origLine;
      l.setP2(l.p2() + delta);
      if (e->modifiers() & Qt::ShiftModifier) 
	l.setP1(l.p1() + delta);
      line->setLine(l);
    }
  }
  e->accept();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  //  unlockBounds();
  ungrabMouse();
  if (resizing) {
    data()->setTextWidth(text->textWidth());
    text->setBoxVisible(false);
  } else {
    if (line) {
      QLineF l = line->line();
      QPointF p0 = mapToParent(l.p1());
      QPointF p1 = l.p2() - l.p1();
      data()->setPos(p0);
      data()->setDelta(p1);
      setPos(p0);
      line->setLine(QLineF(QPointF(0,0), p1));
    } else {
      QPointF p0 = mapToParent(text->pos()
			       - QPointF(0, style().real("note-y-offset")));
      data()->setPos(p0);
      setPos(p0);
    }
    updateTextPos();
  }
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  e->accept();
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  ASSERT(parent);
  GfxNoteData *d = new GfxNoteData(parent->data());
  d->setPos(p0);
  d->setEndPoint(p1);
  d->setTextWidth(0);

  GfxNoteItem *i = new GfxNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}

void GfxNoteItem::childMousePress(QPointF, Qt::MouseButton b, bool resizeFlag) {
  if (mode()->mode()==Mode::MoveResize && b==Qt::LeftButton) {
    text->setFocus();
    text->clearFocus();
    //    lockBounds();
    resizing = resizeFlag;
    if (resizing) {
      qDebug() << "Start note resize";
      initialTextWidth = data()->textWidth();
      if (initialTextWidth<1) {
	initialTextWidth = text->fittedRect().width()+2;
	text->setTextWidth(initialTextWidth);
      }
      qDebug() << "  initial width = " << initialTextWidth;
      text->setBoxVisible(true);
    }
    grabMouse();
  }
}

void GfxNoteItem::makeWritable() {
  text->makeWritable();
  text->setAllowMoves();
}
