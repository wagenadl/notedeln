// TextItemText.C

#include "TextItemText.H"
#include "TextItem.H"
#include <QDebug>

TextItemText::TextItemText(TextItem *parent): QGraphicsTextItem(parent),
					      parent(parent) {
  qDebug() << "TextItemText";
}

TextItemText::~TextItemText() {
}

void TextItemText::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (!parent->mousePress(e))
    QGraphicsTextItem::mousePressEvent(e);
}

void TextItemText::keyPressEvent(QKeyEvent *e) {
  if (!parent->keyPress(e)) 
    QGraphicsTextItem::keyPressEvent(e);    
}

void TextItemText::internalKeyPressEvent(QKeyEvent *e) {
  QGraphicsTextItem::keyPressEvent(e);    
}
  
void TextItemText::focusOutEvent(QFocusEvent *e) {
  if (!parent->focusOut(e)) 
    QGraphicsTextItem::focusOutEvent(e);
}

void TextItemText::focusInEvent(QFocusEvent *e) {
  if (!parent->focusIn(e)) 
    QGraphicsTextItem::focusInEvent(e);
}

