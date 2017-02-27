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

#define MINLINELENGTH 0.1

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
  if (data->text()->lineStarts().isEmpty()) 
    text->document()->relayout();
  else
    text->document()->recalculateCharacterWidths();
  updateTextPos();
}

GfxNoteItem::~GfxNoteItem() {
}

void GfxNoteItem::abandon() {
  BlockItem *ancestor = ancestralBlock();
  Item *p = parent();
  if (p && p->beingDeleted()) {
    qDebug() << "I have a parent but it is being deleted. No toces!";
    return; 
  }
  qDebug() << "GfxNoteItem::abandon" << this << data() << data()->parent();
  if (data()->parent())
    data()->parent()->deleteChild(data());
  deleteLater();
  if (ancestor)
    ancestor->sizeToFit();
}

static double sigmoid(double xl, double xr) {
  double w = xr - xl; // width of box
  double dw = w>20 ? 12 : 3*w/5;
  xl += dw/2;
  w -= dw;
  xl += .1*w;
  w *= .8;
  double x = -xl; // posn of point relative to left of box
  if (x<0)
    return xl;
  x = x * .6;
  if (x>w)
    return xl+w;
  //double a = x/w;
  //constexpr double ALPHA = 2;
  //a = pow(a, ALPHA);
  return xl + x; //+ w*a;
}

QPointF GfxNoteItem::nearestCorner(QPointF pbase, bool *insidep) {
  QRectF docr = text->document()->tightBoundingRect()
    .translated(text->pos() - pbase);

  double dx = style().real("note-x-inset") / 2;
  double dx2 = dx;
  double x0, y0;
  double ygood = docr.top()>0 ? docr.top()
    : docr.bottom()<0 ? -docr.bottom()
    : 0;
  double xgood = docr.left()>0 ? docr.left()
    : docr.right()<0 ? - docr.right()
    : 0;
  if (insidep)
    *insidep = false;

  if (ygood>xgood) {
    // coming mostly from above or below
    if (docr.top() > dx) {
      // coming substantially from above
      y0 = docr.top() - dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.bottom() < -dx) {
      // coming substantially from below
      y0 = docr.bottom() + dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.left() > dx) {
      // coming substantially from the left
      x0 = docr.left() - dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.right() < -dx) {
      // coming substantially from the right
      x0 = docr.right() + dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else {
      // too close by; what can we do that is reasonable?
      x0 = docr.left();
      y0 = docr.top();
      if (insidep)
	*insidep = true;
    }
  } else {
    // coming mostly from left or right
    if (docr.left() > dx) {
      // coming substantially from the left
      x0 = docr.left() - dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.right() < -dx) {
      // coming substantially from the right
      x0 = docr.right() + dx2;
      y0 = sigmoid(docr.top(), docr.bottom());
    } else if (docr.top() > dx) {
      // coming substantially from above
      y0 = docr.top() - dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else if (docr.bottom() < -dx) {
      // coming substantially from below
      y0 = docr.bottom() + dx2;
      x0 = sigmoid(docr.left(), docr.right());
    } else {
      // too close by; what can we do that is reasonable?
      x0 = docr.left();
      y0 = docr.top();
      if (insidep)
	*insidep = true;
    }
  }
  return pbase + QPointF(x0, y0);
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
      if (tw<36)
	tw = 36;
      if (data()->isWritable())
        data()->setTextWidth(tw);
      text->setTextWidth(tw);
    }
  }

  // Arrange line to be shortest
  if (data()->delta().manhattanLength()>MINLINELENGTH) { // minimum line length
    if (!line) {
      line = new QGraphicsLineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
      line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			style().real("note-line-width")));
    }
    bool inside = false;
    QPointF oth = nearestCorner(QPointF(0,0), &inside);
    line->setLine(QLineF(QPointF(0,0), oth));
    if (inside)
      line->hide();
    else 
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
    if ((e->modifiers() & Qt::ShiftModifier)
	&& !(e->modifiers() & Qt::ControlModifier)
	&& !line) {
      /* Create a line only if shift held but not control */
      line = new QGraphicsLineItem(QLineF(QPointF(0,0), QPointF(1,1)), this);
      line->setPen(QPen(QBrush(QColor(style().string("note-line-color"))),
			style().real("note-line-width")));
    }      
    if (line) {
      QLineF l = line->line(); // origLine;
      if ((e->modifiers() & Qt::ShiftModifier)) {
	// leave line's end point in place if shift held
      } else {
	// move line's end point along if shift not held
	l.setP1(l.p1() + delta);
      }
      bool inside = false;
      QPointF oth = nearestCorner(l.p1(), &inside);
      l.setP2(oth);
      line->setLine(l);
      if ((oth-l.p1()).manhattanLength() < MINLINELENGTH || inside) 
	line->hide();
      else
	line->show();
    }
  }
  e->accept();
}

void GfxNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  //  unlockBounds();
  ungrabMouse();
  if (resizing) {
    data()->setTextWidth(text->textWidth());
    updateTextPos();
  } else {
    QPointF ptext = text->pos() - QPointF(0, style().real("note-y-offset"));
    QPointF p0 = mapToParent(ptext);
    if (line && !line->isVisible()) {
      delete line;
      line = 0;
    }
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

void GfxNoteItem::setFocus() {
  text->setFocus();
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


void GfxNoteItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  Item::hoverEnterEvent(e);
  text->setGraphicsEffect(0); // prevent double green
}
