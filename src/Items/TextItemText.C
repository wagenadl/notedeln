// TextItemText.C

#include "TextItemText.H"
#include "TextItem.H"
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include <QGraphicsScene>

TextItemText::TextItemText(TextItem *parent): QGraphicsTextItem(parent) {
  forcebox = false;
}

TextItemText::~TextItemText() {
}

TextItem *TextItemText::parent() {
  return dynamic_cast<TextItem*>(QGraphicsTextItem::parentItem());
}

void TextItemText::mousePressEvent(QGraphicsSceneMouseEvent *e) {
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
  
  if (parent() && !parent()->mousePress(e)) 
    QGraphicsTextItem::mousePressEvent(e);
}

void TextItemText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  if (parent() && !parent()->mouseDoubleClick(e)) 
    QGraphicsTextItem::mouseDoubleClickEvent(e);
}

void TextItemText::keyPressEvent(QKeyEvent *e) {
  if (parent() && !parent()->keyPress(e)) 
    QGraphicsTextItem::keyPressEvent(e);    
}

void TextItemText::internalKeyPressEvent(QKeyEvent *e) {
  QGraphicsTextItem::keyPressEvent(e);    
}
  
void TextItemText::focusOutEvent(QFocusEvent *e) {
  if (parent() && !parent()->focusOut(e)) 
    QGraphicsTextItem::focusOutEvent(e);
}

void TextItemText::focusInEvent(QFocusEvent *e) {
  if (parent() && !parent()->focusIn(e)) 
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
