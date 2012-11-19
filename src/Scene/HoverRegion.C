// HoverRegion.C

#include "HoverRegion.H"

HoverRegion::HoverRegion(class MarkupData *md, class TextItem *item):
  QGraphicsItem(item), md(md), ti(item) {
  popper = 0;
  updateShape(true);
}

HoverRegion::~HoverRegion() {
}

QRectF HoverRegion::boundingRect() const {
  return bounds.boundingRect();
}

void HoverRegion::paint(QPainter *p,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  p->renderPath(bounds); // just for debugging: let's show ourselves
  
}

QPainterPath HoverRegion::shape() const {
  return bounds;
}

void HoverRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    popper->popup();
  else
    popper = new PreviewPopper(md->resMgr(), refText(), this);
}

void HoverRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    delete popper;
  popper = 0;
}

QString HoverRegion::refText() const {
  QTextCursor c(ti->document());
  c.setPosition(md->start());
  c.setPosition(md->end(), QTextCursor::KeepAnchor);
  return c.selectedText();
}

void HoverRegion::updateShape(bool force) {
  if (!force && start==md->start() && end==md->end())
    return;
  
  bounds = QPainterPath();
  int pos = md->start();
  while (pos<md->end()) {
    QTextBlock tb(ti->document()->findBlock(pos));
    QTextLayout *tlay = tb.layout();
    QTextLine line = tlay->lineAt(pos-tb.position());
    double y0 = tlay->position().y() + line.y();
    double y1 = y0 + line.height();
    double x0 = tlay->position().x() + line.cursorToX(pos-tb.position());
    int lineEnd = line.start()+line.length(); 
    double x1 = lineEnd>=md->end()
      ? line.cursorToX(md->end()-tb.position())
      : line.cursorToX(lineEnd);
    bounds.addRect(QRectF(x0, y0, x1, y1));
    pos = tb.position() + lineEnd;
  }

  start = md->start();
  end = md->end();
}
  
