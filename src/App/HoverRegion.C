// HoverRegion.C

#include "HoverRegion.H"
#include "TextItem.H"
#include "PreviewPopper.H"
#include "ResManager.H"
#include "ResourceMagic.H"
#include "Assert.H"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextLine>
#include <QDebug>

HoverRegion::HoverRegion(class MarkupData *md, class TextItem *item,
			 QGraphicsItem *parent):
  QGraphicsObject(parent), md(md), ti(item) {
  start = end = -1;
  popper = 0;
  busy = false;
  lastRef = "";
  setAcceptHoverEvents(true);
}

HoverRegion::~HoverRegion() {
}

QRectF HoverRegion::boundingRect() const {
  calcBounds();
  return bounds.boundingRect();
}

void HoverRegion::paint(QPainter *p,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  if (!hasArchive() || !hasPreview()) {
    if (ti->isWritable() /* || ti->style().bool("fetch-old-links") */) {
      getArchiveAndPreview();
    }
  }
  calcBounds();
  p->setPen(Qt::NoPen);
  QColor c;
  if (hasArchive() || hasPreview()) {
    c = ti->style().color("hover-found-color");
    c.setAlpha(int(255*ti->style().real("hover-found-alpha")));
  } else {
    c = ti->style().color("hover-not-found-color");
    c.setAlpha(int(255*ti->style().real("hover-not-found-alpha")));
  }    
  p->setBrush(c);
  p->drawPath(bounds); // just for debugging: let's show ourselves
}

QPainterPath HoverRegion::shape() const {
  calcBounds();
  return bounds;
}

void HoverRegion::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (e->modifiers() & Qt::ControlModifier) {
    e->accept();
    qDebug() << "HoverRegion: control mouse press";
    switch (md->style()) {
    case MarkupData::Link:
      if (e->modifiers() & Qt::ShiftModifier || !hasArchive()) 
	openLink();
      else 
	openArchive();
      break;
    default:
      qDebug() << "HoverRegion: Don't know how to open this markup"
	       << md->style() << refText();
      break;
    }
  } else {
    e->ignore();
  }
}

void HoverRegion::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  if (e->modifiers() & Qt::ControlModifier) {
    e->accept();
    qDebug() << "HoverRegion: mouse double click";
  } else {
    e->ignore();
  }
}

void HoverRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  if (popper) {
    popper->popup();
    return;
  }
  QString txt = refText();
  if (txt.startsWith("www."))
    txt = "http://" + txt;
  Resource *r = md->resManager()->byTag(txt);
  if (!r)
    r = md->resManager()->byURL(txt);
  if (r)
    popper = new PreviewPopper(r, e->screenPos(), this);
}

void HoverRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    popper->deleteLater();
  popper = 0;
}

Resource *HoverRegion::resource() const {
  ResManager *resmgr = md->resManager();
  if (!resmgr) {
    qDebug() << "HoverRegion: no resource manager";
    return 0;
  }
  return resmgr->byTag(refText());
}

bool HoverRegion::hasArchive() const {
  Resource *res = resource();
  return res ? res->hasArchive() : false;
}

bool HoverRegion::hasPreview() const {
  Resource *res = resource();
  return res ? res->hasPreview() : false;
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
    ASSERT(tb.isValid());
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
  if (resource() && !md->resourceTags().contains(refText())) 
    md->attachResource(refText());
}
  
void HoverRegion::openLink() {
  Resource *r = resource();
  if (r)
    qDebug() << "HoverRegion: openURL" << r->sourceURL();
  else
    qDebug() << "HoverRegion: openURL" << refText() <<  "(no url)";
}

void HoverRegion::openArchive() {
  Resource *r = resource();
  if (r)
    qDebug() << "HoverRegion: openArchive" << r->archivePath();
  else
    qDebug() << "HoverRegion: openArchive" << refText() << "(no arch)";
}  

void HoverRegion::getArchiveAndPreview() {
  qDebug() << "HoverRegion: getArchiveAndPreview" << refText() << lastRef << busy;
  if (refText()==lastRef || busy)
    return; // we know we can't do it

  ResManager *resmgr = md->resManager();
  if (!resmgr) {
    qDebug() << "HoverRegion: no resource manager";
    return;
  }
  if (refText()!=lastRef && !lastRef.isEmpty())
    md->detachResource(lastRef);
  
  lastRef = refText();
  Resource *r = resmgr->byTag(lastRef);
  if (r) {
    lastRefIsNew = false;
  } else {
    r = md->resManager()->newResource(lastRef);
    lastRefIsNew = true;
  }
  ResourceMagic::magicLink(r);
  connect(r, SIGNAL(finished()), SLOT(downloadFinished()));
  busy = true;
  r->getArchiveAndPreview();
}

void HoverRegion::downloadFinished() {
  qDebug() << "HoverRegion::downloadFinished";
  ASSERT(busy);
  if (refText()!=lastRef) {
    // we have already changed; so we're not interested in the results
    // anymore
    if (lastRefIsNew) {
      ResManager *resmgr = md->resManager();
      Resource *r = resmgr->byTag(lastRef);
      resmgr->dropResource(r);
    }
  } else {
    if (hasArchive() || hasPreview()) {
      // at least somewhat successful
      qDebug() << "Attaching new resource" << lastRef;
      md->attachResource(lastRef);
      update();
    } else {
      // failure
      if (lastRefIsNew) {
	ResManager *resmgr = md->resManager();
	Resource *r = resmgr->byTag(lastRef);
	resmgr->dropResource(r);
      }
    }
  }
  busy = false;
}
