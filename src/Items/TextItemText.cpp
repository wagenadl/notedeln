// Items/TextItemText.cpp - This file is part of eln

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

// TextItemText.C

#include "TextItemText.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include <QGraphicsScene>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

TextItemText::TextItemText(TextItem *parent): QGraphicsTextItem(parent) {
  forcebox = false;
}

TextItemText::~TextItemText() {
}

TextItem *TextItemText::parent() {
  return dynamic_cast<TextItem*>(QGraphicsTextItem::parentItem());
}

void TextItemText::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "TIT: mousepress";
  // Following is an ugly way to clear selection from previously
  // focused text. Can I do better?
  foreach (QGraphicsItem *i, scene()->items()) {
    QGraphicsTextItem *gti = dynamic_cast<QGraphicsTextItem*>(i);
    if (gti && gti!=this) {
      QTextCursor c = gti->textCursor();
      if (c.hasSelection()) {
        c.clearSelection();
        gti->setTextCursor(c);
        break;
      }
    }
  }
  // End of ugly code
  
  if (!parent() || !parent()->mousePress(e)) 
    QGraphicsTextItem::mousePressEvent(e);
}

void TextItemText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  if (!parent() || !parent()->mouseDoubleClick(e)) 
    QGraphicsTextItem::mouseDoubleClickEvent(e);
}

void TextItemText::keyPressEvent(QKeyEvent *e) {
  if (!parent() || !parent()->keyPress(e)) 
    QGraphicsTextItem::keyPressEvent(e);
  ensureFocusVisible();
}

void TextItemText::ensureFocusVisible() {
  if (!hasFocus())
    return;
  if (clipp.isEmpty())
    return;
  QPointF c = posToPoint(this, textCursor().position());
  qDebug() << "ensureFocusVisible " << c << c.isNull() << (clipp.contains(c) ? "Y" : "N");
  if (!clipp.contains(c))
    emit invisibleFocus(c);
}

void TextItemText::internalKeyPressEvent(QKeyEvent *e) {
  QGraphicsTextItem::keyPressEvent(e);    
}
  
void TextItemText::focusOutEvent(QFocusEvent *e) {
  if (!parent() || !parent()->focusOut(e)) 
    QGraphicsTextItem::focusOutEvent(e);
}

void TextItemText::focusInEvent(QFocusEvent *e) {
  if (!parent() || !parent()->focusIn(e)) 
    QGraphicsTextItem::focusInEvent(e);
}

void TextItemText::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  if (parent())
    parent()->hoverMove(e);
} 

void TextItemText::setBoxVisible(bool v) {
  qDebug() << "TextItemText" << v;
  forcebox = v;
  update();
}

void TextItemText::paint(QPainter * p,
			 const QStyleOptionGraphicsItem *s,
			 QWidget *w) {
  if (forcebox) {
    QStyleOptionGraphicsItem sogi(*s);
    sogi.state |= QStyle::State_HasFocus;
    QGraphicsTextItem::paint(p, &sogi, w);
  } else {
    QGraphicsTextItem::paint(p, s, w);
  }
}

int pointToPos(QGraphicsTextItem const *item, QPointF p) {
  QTextDocument *doc = item->document();
  for (QTextBlock b = doc->begin(); b!=doc->end(); b=b.next()) {
    QTextLayout *lay = b.layout();
    qDebug() << "p2p: layR = " << lay->boundingRect() << lay->position() << " b.pos=" << b.position();
    QPointF p1 = p - lay->position();
    if (lay->boundingRect().contains(p1)) {
      int nLines = lay->lineCount();
      for (int i=0; i<nLines; i++) {
	QTextLine line = lay->lineAt(i); // yes, this returns the i-th line
	if (line.rect().contains(p1)) 
	  return line.xToCursor(p1.x()) + b.position();
      }
      qDebug() << "TextItem: point in block but not in a line!?";
      return -1;
    }
  }
  return -1;
}

QPointF posToPoint(QGraphicsTextItem const *item, int i) {
  QTextBlock blk = item->document()->findBlock(i);
  qDebug() << "posToPoint" << i << blk.isValid();
  if (!blk.isValid())
    return QPointF();
  QTextLayout *lay = blk.layout();
  if (!lay)
    return QPointF();
  qDebug() << lay->isValidCursorPosition(i - blk.position());
  if (!lay->isValidCursorPosition(i - blk.position())) {
    if (lay->lineCount()==0)
      return QPointF();
    QTextLine line = lay->lineAt(lay->lineCount()-1);
    QPointF p(line.cursorToX(line.textLength()), line.y()+line.ascent()/2);
    return p + lay->position();
  }
  QTextLine line = lay->lineForTextPosition(i - blk.position());
  qDebug() << line.isValid();
  if (!line.isValid())
    return QPointF();
  QPointF p(line.cursorToX(i-blk.position()), line.y()+line.ascent()/2);
  qDebug() << p << line.y() << line.ascent() << line.descent() << line.rect();
  qDebug() << "postopoint ok";
  return p + lay->position();
}

void TextItemText::dragEnterEvent(QGraphicsSceneDragDropEvent *e) {
  qDebug() << "TextItemText::dragEnter";
  if (e->mimeData()->hasText()) 
    QGraphicsTextItem::dragEnterEvent(e);
  // we don't care about non-text and we will not allow Qt to care either
}

void TextItemText::dropEvent(QGraphicsSceneDragDropEvent *e) {
  qDebug() << "TextItemText::dropEvent";
  //int p = pointToPos(this, e->pos());
  if (!e->mimeData()->hasText())
    return;
  QString txt = e->mimeData()->text();
  QMimeData m;
  m.setText(txt);
  e->setMimeData(&m);
  QGraphicsTextItem::dropEvent(e);
}

void TextItemText::setClip(QRectF r) {
  QRectF r0 = boundingRect();
  clipp = QPainterPath();
  clipp.addRect(QRectF(r0.left(), r.top(), r0.width(), r.height()));
  setFlag(ItemClipsToShape, true);
}

void TextItemText::unclip() {
  clipp = QPainterPath();
  setFlag(ItemClipsToShape, false);
}

QPainterPath TextItemText::shape() const {
  return clipp.isEmpty() ? QGraphicsTextItem::shape() : clipp;
}
