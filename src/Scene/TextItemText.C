// TextItemText.C

#include "TextItemText.H"
#include "TextItem.H"
#include <QDebug>

TextItemText::TextItemText(TextItem *parent): QGraphicsTextItem(parent) {
}

TextItemText::~TextItemText() {
}

TextItem *TextItemText::parent() {
  return dynamic_cast<TextItem*>(QGraphicsTextItem::parent());
}

void TextItemText::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (parent() && !parent()->mousePress(e))
    QGraphicsTextItem::mousePressEvent(e);
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

