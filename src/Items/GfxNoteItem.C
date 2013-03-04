// GfxNoteItem.C

#include "GfxNoteItem.H"
#include "GfxNoteData.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include "Assert.H"
#include "BlockItem.H"
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  line = 0;
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

static double euclideanLength2(QPointF p) {
  return p.x()*p.x() + p.y()*p.y();
}

QPointF GfxNoteItem::nearestCorner(QPointF pbase) {
  double yof = style().real("note-y-offset");
  QTextBlock b = text->document()->firstBlock();
  QTextLayout *lay = b.layout();
  QPointF p0 = lay->position() + text->pos() - pbase;
  if (lay->lineCount()==0) { // this shouldn't happen, I think
    return pbase;
  } else {
    QRectF l0rect = lay->lineAt(0).naturalTextRect();
    l0rect.translate(p0);
    QRectF lnrect = lay->lineAt(lay->lineCount()-1).naturalTextRect();
    lnrect.translate(p0);
    QPointF tl = l0rect.topLeft() + QPointF(-3, -yof);
    QPointF tr = l0rect.topRight() + QPointF(3, -yof);
    QPointF bl = lnrect.topLeft() + QPointF(-3, -yof);
    QPointF br = lnrect.topRight() + QPointF(3, -yof);
    QList<QPointF> pl; pl << tl << tr << bl << br;
    int idx = -1;
    double dmin = 0;
    for (int i=0; i<4; i++) {
      double d = euclideanLength2(pl[i]);
      if (idx<0 || d<dmin) {
	idx = i;
	dmin = d;
      }
    }
    return pbase + pl[idx];
  }
}
  

void GfxNoteItem::updateTextPos() {
  // Position text at delta (with note offset for compatibility)
  QPointF p = data()->delta();
  double yof = style().real("note-y-offset");
  p += QPointF(0, yof);
  text->setPos(p);

  // Auto limit text width
  if (data()->textWidth()<1) {
    QRectF sr = text->mapRectToScene(text->fittedRect());
    if (sr.right() >= style().real("page-width")
	- style().real("margin-right-over")) {
      double tw = style().real("page-width")
	- style().real("margin-right-over")
	- sr.left();
      data()->setTextWidth(tw);
      text->setTextWidth(tw);
    }
  }

  // Arrange line to be shortest
  if (data()->delta().manhattanLength()>2) { // minimum line length
    if (!line) {
      line = new QGraphicsLineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
      line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			style().real("note-line-width")));
    }
    line->setLine(QLineF(QPointF(0,0), nearestCorner()));
    line->show();
  } else {
      if (line)
	line->hide();
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
    if (e->modifiers() & Qt::ShiftModifier && !line) {
      line = new QGraphicsLineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
      line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			style().real("note-line-width")));
    }      
    if (line) {
      QLineF l = line->line(); // origLine;
      if (e->modifiers() & Qt::ShiftModifier) 
	l.setP1(l.p1() + delta);
      l.setP2(nearestCorner(l.p1()));
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
    updateTextPos();
  } else {
    QPointF ptext = text->pos() - QPointF(0, style().real("note-y-offset"));
    QPointF p0 = mapToParent(ptext);
    if (line) {
      QPointF pbase = line->line().p1();
      p0 = mapToParent(pbase);
      data()->setDelta(ptext - pbase);
    } else {
      data()->setDelta(QPointF(0, 0));
    }
    data()->setPos(p0);
    setPos(p0);
    updateTextPos();
  }
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  e->accept();
}

GfxNoteItem *GfxNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  ASSERT(parent);
  GfxNoteData *d = new GfxNoteData(parent->data());
  QPointF sp0 = parent->mapToScene(p0);
  QPointF sp1 = parent->mapToScene(p1);
  d->setPos(p0);
  d->setDelta(sp1-sp0);
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

void GfxNoteItem::setScale(qreal f) {
  Item::setScale(f);
  updateTextPos();
}
