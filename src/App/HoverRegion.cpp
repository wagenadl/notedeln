// App/HoverRegion.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// HoverRegion.C

#include "HoverRegion.H"
#include "TextItem.H"
#include "PreviewPopper.H"
#include "ResManager.H"
#include "ResourceMagic.H"
#include "Assert.H"
#include "SheetScene.H"
#include "PageView.H"
#include "TextData.H"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextLine>
#include <QDebug>
#include <QProcess>

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
  p->setClipRect(ti->clipRect());
  p->drawPath(bounds);
}

QPainterPath HoverRegion::shape() const {
  calcBounds();
  return bounds;
}

void HoverRegion::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (ti->mode()->mode()==Mode::Browse
      || (e->modifiers() & Qt::ControlModifier)) 
    activate(e);
  else
    e->ignore();
}

void HoverRegion::activate(QGraphicsSceneMouseEvent *e) {
  e->accept();
  qDebug() << "HoverRegion::activate event from " << e->widget();
  switch (md->style()) {
  case MarkupData::Link:
    if (e->modifiers() & Qt::ShiftModifier)
      openLink();
    else 
      openArchive();
    break;
  default:
    qDebug() << "HoverRegion: Don't know how to open this markup"
	       << md->style() << refText();
    break;
  }
}

void HoverRegion::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  activate(e);
}

void HoverRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  if (popper) {
    popper->popup();
    return;
  }
  QString txt = refText();
  if (txt.isEmpty())
    return;
  Resource *r = resource();
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
  prepareGeometryChange();
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
    if (!line.isValid()) {
      qDebug() << "Invalid line";
      qDebug() << "md:" << md->start() << md->end();
      qDebug() << "pos:" << pos << "rpos:" << rpos;
      qDebug() << "data length: " << ti->data()->text().size();
      break;
    }
    double y0 = tlay->position().y() + line.y();
    double y1 = y0 + line.height();
    double x0 = tlay->position().x() + line.cursorToX(rpos);
    int lineEnd = line.textStart()+line.textLength();
    int rend = md->end()-tb.position();
    double x1 = tlay->position().x() + (rend<lineEnd
					? line.cursorToX(rend)
					: line.cursorToX(lineEnd));
    QRectF bit(QPointF(x0, y0), QPointF(x1, y1));
    if (!bit.isEmpty()) {
      bit = mapRectFromScene(ti->mapRectToScene(bit));
      bounds.addRect(bit);
    }
    int npos = tb.position() + lineEnd;
    if (npos<=pos)
      break;
    pos = npos;
  }

  start = md->start();
  end = md->end();
  if (resource() && !md->resourceTags().contains(refText())) 
    md->attachResource(refText());
}
  
void HoverRegion::openLink() {
  Resource *r = resource();
  if (!r) {
    qDebug() << "HoverRegion: openURL" << refText() <<  "(no url)";
    return;
  }
  qDebug() << "HoverRegion: openURL" << r->sourceURL();
  if (r->sourceURL().scheme() == "page") {
    openPage(true);
  } else {
    QStringList args;
    args << r->sourceURL().toString();
    bool ok = QProcess::startDetached("gnome-open", args);
    if (!ok)
      qDebug() << "Failed to start external process 'gnome-open'";
  }
}

void HoverRegion::openPage(bool newView) {
  Resource *r = resource();
  ASSERT(r);
  QString tag = r->tag();

  ASSERT(ti);
  SheetScene *s = dynamic_cast<SheetScene *>(ti->scene());
  ASSERT(s);
  PageView *pv = dynamic_cast<PageView *>(s->eventView());
  ASSERT(pv);

  if (newView)
    pv->newView(tag);
  else
    pv->gotoEntryPage(tag);
}

void HoverRegion::openArchive() {
  Resource *r = resource();
  if (!r) {
    qDebug() << "HoverRegion: openArchive" << refText() << "(no arch)";
    return;
  }
  if (!hasArchive()) {
    openLink();
    return;
  }

  qDebug() << "HoverRegion: openArchive" << r->archivePath();
  if (r->sourceURL().scheme() == "page") {
    openPage();
  } else {
    QStringList args;
    args << r->archivePath();
    bool ok = QProcess::startDetached("gnome-open", args);
    if (!ok)
      qDebug() << "Failed to start external process 'gnome-open'";
  }
}  

void HoverRegion::getArchiveAndPreview() {
  qDebug() << "HoverRegion::getArchiveAndPreview" << refText() << lastRef;

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
  connect(r, SIGNAL(finished()), SLOT(downloadFinished()));
  busy = true;
  r->getArchiveAndPreview();
}

void HoverRegion::downloadFinished() {
  qDebug() << "HoverRegion::downloadFinished" << refText();
  if (!busy) {
    qDebug() << "not busy";
    return;
  }
  ASSERT(busy);
  ResManager *resmgr = md->resManager();
  Resource *r = resmgr->byTag(lastRef);
  if (!r || refText()!=lastRef) {
    /* Either the resource got destroyed somehow, or we have already
       changed; so we're not interested in the results anymore. */
    if (lastRefIsNew) 
      resmgr->dropResource(r);
  } else if (r->hasArchive() || r->hasPreview()
             || !r->title().isEmpty() || !r->description().isEmpty()) {
    // at least somewhat successful
    qDebug() << "Attaching new resource" << lastRef;
    md->attachResource(lastRef);
    update();
  } else {
    // utter failure
    if (lastRefIsNew) {
      resmgr->dropResource(r);
    }
  }
  busy = false;
}
