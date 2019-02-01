// Items/GfxBlockItem.cpp - This file is part of eln

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

// GfxBlockItem.C

#include "GfxBlockItem.h"
#include "GfxBlockData.h"
#include "Mode.h"
#include "EntryScene.h"
#include "Style.h"
#include <QPainter>
#include <QDebug>
#include "ResManager.h"
#include "TextItem.h"
#include "GfxImageData.h"
#include "GfxImageItem.h"
#include "GfxNoteData.h"
#include "GfxNoteItem.h"
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include "SvgFile.h"
#include <QMimeData>
#include <QCursor>
#include "GfxMarkItem.h"
#include "GfxSketchItem.h"
#include "GfxLineItem.h"
#include "Assert.h"
#include "Cursors.h"
#include "Notebook.h"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, Item *parent):
  BlockItem(data, parent) {

  setPos(style().real("margin-left"), 0); // doing this first aids LateNoteItems

  foreach (GfxData *g, data->gfx()) 
    create(g, this);
}

GfxBlockItem::~GfxBlockItem() {
}

static QPointF constrainPointToRect(QPointF p, QRectF rect) {
  if (p.x()<rect.left())
    p.setX(rect.left());
  else if (p.x()>rect.right())
    p.setX(rect.right());
  if (p.y()<rect.top())
    p.setY(rect.top());
  else if (p.y()>rect.bottom())
    p.setY(rect.bottom());
  return p;
}

Item *GfxBlockItem::newImage(QImage img, QUrl src, QPointF pos) {
  ASSERT(data()->book());
  ASSERT(data()->resManager());
  double maxW = availableWidth();
  double maxH = maxW;
  double scale = 1;
  if (scale*img.width()>maxW)
    scale = maxW/img.width();
  if (scale*img.height()>maxH)
    scale = maxH/img.height();
  if (allChildren().isEmpty())
    pos = QPointF(0, 0);
  else
    pos -= QPointF(img.width(),img.height())*(scale/2);
  pos = constrainPointToRect(pos, boundingRect());
  Resource *res = data()->resManager()->importImage(img, src);
  QString resName = res->tag();
  GfxImageData *gid = new GfxImageData(resName, img, data());
  gid->setScale(scale);
  gid->setPos(pos);
  GfxImageItem *gii = new GfxImageItem(gid, this);
  gii->makeWritable();
  resetPosition();
  sizeToFit();
  return gii;
}

double GfxBlockItem::availableWidth() const {
  return style().real("page-width") -
    style().real("margin-left") -
    style().real("margin-right");
}

QRectF GfxBlockItem::generousChildrenBounds() const {
  QRectF r;
  foreach (Item *i, allChildren())
    if (!i->excludeFromNet())
      r |= i->mapRectToParent(i->netBounds());
  if (r.isEmpty())
    return r;
  r.setTop(r.top()-style().real("gfx-block-vmargins"));
  r.setBottom(r.bottom()+style().real("gfx-block-vmargins"));
  return r;
}

void GfxBlockItem::sizeToFit() {
  QRectF r = generousChildrenBounds();
  double minh = style().real("gfx-block-minh");
  if (r.height() < minh)
    r.setHeight(minh);

  double yref = data()->yref();
  double h = data()->height();

  bool mustemit = false;
  
  if (yref!=r.top()) {
    prepareGeometryChange();
    if (isWritable())
      data()->setYref(r.top());
    else
      data()->sneakilySetYref(r.top());
    mustemit = true;
  }
  if (h!=r.height()) {
    prepareGeometryChange();
    if (isWritable())
      data()->setHeight(r.height());
    else
      data()->sneakilySetHeight(r.height());        
    mustemit = true;
  }

  if (mustemit)
    emit heightChanged();
}

QRectF GfxBlockItem::boundingRect() const {
  /* This returns the bounds of our grid and has nothing to do with children.
   */
  return QRectF(0, data()->yref(), availableWidth(), data()->height());
}
   
void GfxBlockItem::paint(QPainter *p,
			 const QStyleOptionGraphicsItem *,
			 QWidget *) {
  // paint background grid; items draw themselves  
  QRectF bb = boundingRect();
  QColor c(style().color("canvas-grid-color"));
  c.setAlphaF(style().real("canvas-grid-alpha"));
  p->setPen(QPen(c,
		 style().real("canvas-grid-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double dx = style().real("canvas-grid-spacing");
  drawGrid(p, bb, dx);

  c = style().color("canvas-grid-major-color");
  c.setAlphaF(style().real("canvas-grid-major-alpha"));

  p->setPen(QPen(c,
		 style().real("canvas-grid-major-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  dx *= style().integer("canvas-grid-major-interval");
  if (dx)
    drawGrid(p, bb, dx);
}

void GfxBlockItem::drawGrid(QPainter *p, QRectF const &bb, double dx) {
  double x0 = dx*ceil(bb.left()/dx);
  double x1 = dx*floor(bb.right()/dx);
  double y0 = dx*ceil(bb.top()/dx);
  double y1 = dx*floor(bb.bottom()/dx);
  for (double x = x0; x<=x1+.001; x+=dx)
    p->drawLine(x, bb.top(), x, bb.bottom());
  for (double y = y0; y<=y1+.001; y+=dx)
    p->drawLine(bb.left(), y, bb.right(), y);
}

bool GfxBlockItem::perhapsSendMousePressToChild(QGraphicsSceneMouseEvent *) {
  /* Idea is to find a child item that is near the mouse press position, and
     send the press event to it in the hopes of creating a drag for it. I don't
     know if this is really practical: Somehow, subsequent mouse move and
     mouse release events would have to be forwarded as well.
  */
  return false; // make Control-press do nothing.
}
  

void GfxBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  Mode::M mod = mode()->mode();
  //  Qt::MouseButton but = e->button();
  bool take = false;
  if (isWritable()) {
    switch (mod) {
    case Mode::Mark:
      GfxMarkItem::newMark(e->pos(), this);
      sizeToFit();
      take = true;
      break;
    case Mode::Freehand: {
      if (mode()->isStraightLineMode()) {
	GfxLineItem *li = GfxLineItem::newLine(e->pos(), this);
	li->build(e);
      } else {
	GfxSketchItem *ski = GfxSketchItem::newSketch(e->pos(), this);
	ski->build();
      }
      take = true;
    } break;
    case Mode::Type:
      if (e->modifiers() & Qt::ControlModifier) {
	take = perhapsSendMousePressToChild(e);
      } else {
	createGfxNote(e->pos());
	take = true;
      }
      break;
    default:
      break;
    }
  }    

  if (take) 
    e->accept();
  else
    BlockItem::mousePressEvent(e);
}

void GfxBlockItem::makeWritable() {
  BlockItem::makeWritable();
  setAcceptDrops(true);
}

void GfxBlockItem::dragEnterEvent(QGraphicsSceneDragDropEvent *e) {
  QMimeData const *md = e->mimeData();
  qDebug() << "GfxBlockItem::dragEnterEvent: has image? " << md->hasImage()
	   << "hasurl?" << md->hasUrls()
	   << "hastext?" << md->hasText()
	   << "proposed" << e->proposedAction()
	   << "iswritable" << isWritable();
  if (isWritable() && (md->hasImage() || md->hasUrls() || md->hasText())) {
    e->setDropAction(Qt::CopyAction);
    setCursor(Cursors::crossCursor());
  } else {
    e->ignore();
  }
}

void GfxBlockItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *e) {
  BlockItem::dragLeaveEvent(e);
  setCursor(cursorShape(e->modifiers()));
}

bool GfxBlockItem::changesCursorShape() const {
  return true;
}

Qt::CursorShape GfxBlockItem::cursorShape(Qt::KeyboardModifiers) const {
  Qt::CursorShape cs = defaultCursorShape();
  switch (mode()->mode()) {
  case Mode::Annotate:
    cs = Qt::CrossCursor;
  break;
  case Mode::Type: case Mode::Mark: case Mode::Freehand:
    if (isWritable())
      cs = Qt::CrossCursor;
    break;
  default:
    break;
  }
  return cs;
}

void GfxBlockItem::dropEvent(QGraphicsSceneDragDropEvent *e) {
  QMimeData const *md = e->mimeData();
  if (md->hasImage()) {
    QUrl url;
    if (md->hasUrls()) {
      QList<QUrl> uu = md->urls();
      if (!uu.isEmpty())
	url = uu[0];
    }
    newImage(qvariant_cast<QImage>(md->imageData()), url, e->pos());
    e->setDropAction(Qt::CopyAction);
  } else if (md->hasUrls()) {
    foreach (QUrl const &u, md->urls())
      importDroppedUrl(u, e->pos());
    e->setDropAction(Qt::CopyAction);
  } else if (md->hasText()) {
    importDroppedText(md->text(), e->pos());
   e->setDropAction(Qt::CopyAction);
  }
}

void GfxBlockItem::importDroppedText(QString txt, QPointF p) {
  GfxNoteItem *note = newGfxNote(p, p);
  note->textItem()->textCursor().insertText(txt);
}

void GfxBlockItem::importDroppedUrl(QUrl const &url, QPointF p) {
  if (url.isLocalFile()) {
    QString path = url.toLocalFile();
    if (path.endsWith(".svg")) {
       importDroppedSvg(url, p);
       return;
    }
    QImage image(path);
    if (!image.isNull())
      newImage(image, url, p);
    else 
      importDroppedText(path, p); // import filename as text
  } else {
    // Right now, we import all network urls as text
    importDroppedText(url.toString(), p);
  }
}

void GfxBlockItem::importDroppedSvg(QUrl const &url, QPointF p) {
  QImage img(SvgFile::downloadAsImage(url));
  if (img.isNull()) 
    importDroppedText(url.toString(), p);
  else
    newImage(img, url, p);
}
