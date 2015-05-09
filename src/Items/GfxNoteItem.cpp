// Items/GfxNoteItem.cpp - This file is part of eln

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

// GfxNoteItem.C

#include "GfxNoteItem.h"
#include "GfxNoteData.h"
#include "TextItem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include "Assert.h"
#include "BlockItem.h"
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>
#include <math.h>

static Item::Creator<GfxNoteData, GfxNoteItem> c("gfxnote");

GfxNoteItem::GfxNoteItem(GfxNoteData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  line = 0;
  text = new TextItem(data->text(), this);
  connect(text, SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  SLOT(futileMovementKey(int, Qt::KeyboardModifiers)));
  text->setFont(style().font("note-font"));
  text->setDefaultTextColor(QColor(style().string("note-text-color")));
  if (data->textWidth()>1)
    text->setTextWidth(data->textWidth(), false);

  text->setLineHeight(style().lineSpacing("note-font",
					  "note-line-spacing"));
  
  connect(text, SIGNAL(abandoned()),
	  this, SLOT(abandon()), Qt::QueuedConnection);
		     
  setFlag(ItemIsFocusable);
  connect(text->document(), SIGNAL(contentsChanged(int, int, int)),
	  SLOT(updateTextPos()));
  updateTextPos();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::abandon() {
  qDebug() << "GfxNoteItem::abandon";
  BlockItem *ancestor = ancestralBlock();
  Item *p = parent();
  qDebug() << "  I am " << this << ". My parent is " << p
	   << ". My ancestor is " << ancestor;
  if (p && p->beingDeleted()) {
    qDebug() << "I have a parent but it is being deleted. No toces!";
    return; 
  }
  if (p)
    p->data()->deleteChild(data());
  deleteLater();
  if (ancestor)
    ancestor->sizeToFit();
  qDebug() << "  Abandon complete";
}

static double euclideanLength2(QPointF p) {
  return p.x()*p.x() + p.y()*p.y();
}

static QPointF retract(QPointF p, double d) {
  double l = sqrt(euclideanLength2(p))+.001;
  QPointF p0 = (1/l) * p; // normalized
  return p - d * p0;
}

QPointF GfxNoteItem::nearestCorner(QPointF pbase) {
  double yof = style().real("note-y-offset");
  double xof = style().real("note-x-inset");
  QRectF docr = text->document()->boundingRect()
    .adjusted(xof, -yof, -xof, -yof).translated(text->pos() - pbase);

  QPointF p;
  if (fabs(docr.top()) < fabs(docr.bottom())) {
    if (docr.left()>0)
      p = retract(docr.topLeft(), xof*1.5);
    else if (docr.right()<0)
      p = retract(docr.topRight(), xof*1.5);
    else
      p = retract((docr.topLeft()+docr.topRight())/2, xof*1.5);
  } else {
    if (docr.left()>0)
      p = retract(docr.bottomLeft(), xof*1.5);
    else if (docr.right()<0)
      p = retract(docr.bottomRight(), xof*1.5);
    else
      p = retract((docr.bottomLeft()+docr.bottomRight())/2, xof*1.5);
  }
  
  return pbase + p;
}
  

void GfxNoteItem::updateTextPos() {
  // Position text at delta (with note offset for compatibility)
  QPointF p = data()->delta();
  double yof = style().real("note-y-offset");
  p += QPointF(0, yof);
  text->setPos(p);

  // Auto limit text width
  if (data()->textWidth()<1) {
    QRectF sr = text->mapRectToScene(text->netBounds());
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
      initialTextWidth = data()->textWidth();
      if (initialTextWidth<1) {
	initialTextWidth = text->netBounds().width()+2;
	text->setTextWidth(initialTextWidth);
      }
      text->setBoxVisible(true);
    }
    grabMouse();
  }
}

void GfxNoteItem::makeWritable() {
  Item::makeWritable();
  text->setAllowMoves();
}

void GfxNoteItem::setScale(qreal f) {
  Item::setScale(f);
  updateTextPos();
}

void GfxNoteItem::translate(QPointF dxy) {
  data()->setPos(data()->pos() + dxy);
  setPos(pos() + dxy);
}


void GfxNoteItem::futileMovementKey(int k, Qt::KeyboardModifiers) {
  switch (k) {
  case Qt::Key_Left: case Qt::Key_Up: {
    TextCursor c(text->textCursor());
    c.movePosition(TextCursor::Start);
    text->setTextCursor(c);
  } break;
  case Qt::Key_Right: case Qt::Key_Down: {
    TextCursor c(text->textCursor());
    c.movePosition(TextCursor::End);
    text->setTextCursor(c);
  } break;
  default:
    break;
  }
}


