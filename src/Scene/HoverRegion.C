// HoverRegion.C

#include "HoverRegion.H"
#include "TextItem.H"
#include "PreviewPopper.H"
#include "ResourceManager.H"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextLine>
#include <QDebug>

HoverRegion::HoverRegion(class MarkupData *md, class TextItem *item):
  QGraphicsObject(item), md(md), ti(item) {
  start = end = -1;
  popper = 0;
  setAcceptHoverEvents(true);
}

HoverRegion::~HoverRegion() {
}

QRectF HoverRegion::boundingRect() const {
  calcBounds();
  return bounds.boundingRect();
}

void HoverRegion::paint(QPainter *,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  //calcBounds();
  //p->drawPath(bounds); // just for debugging: let's show ourselves
  
}

QPainterPath HoverRegion::shape() const {
  calcBounds();
  return bounds;
}

void HoverRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  if (popper) {
    popper->popup();
    return;
  }
  QString txt = refText();
  if (txt.startsWith("www."))
    txt = "http://" + txt;
  QString resname = md->resMgr()->contains(txt)
    ? txt
    : md->resMgr()->resName(txt);
  popper = new PreviewPopper(md->resMgr(), resname, e->screenPos(), this);
}

void HoverRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    popper->deleteLater();
  popper = 0;
}

QString HoverRegion::refText() const {
  QTextCursor c(ti->document());
  c.setPosition(md->start());
  c.setPosition(md->end(), QTextCursor::KeepAnchor);
  return c.selectedText();
}

void HoverRegion::forgetBounds() {
  start = end = -1;
  update();
}

void HoverRegion::calcBounds() const {
  if (start==md->start() && end==md->end())
    return; // old value will do
  bounds = QPainterPath();
  int pos = md->start();
  while (pos<md->end()) {
    QTextBlock tb(ti->document()->findBlock(pos));
    Q_ASSERT(tb.isValid());
    int rpos = pos-tb.position();
    QTextLayout *tlay = tb.layout();
    QTextLine line = tlay->lineForTextPosition(rpos);
    double y0 = tlay->position().y() + line.y();
    double y1 = y0 + line.height();
    double x0 = tlay->position().x() + line.cursorToX(rpos);
    int lineEnd = line.textStart()+line.textLength();
    int rend = md->end()-tb.position();
    double x1 = tlay->position().x() + (rend<lineEnd
					? line.cursorToX(rend)
					: line.cursorToX(lineEnd));
    bounds.addRect(QRectF(QPointF(x0, y0), QPointF(x1, y1)));
    pos = tb.position() + lineEnd;
  }

  start = md->start();
  end = md->end();
}
  
