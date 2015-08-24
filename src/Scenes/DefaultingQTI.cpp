// DefaultingQTI.cpp

#include "DefaultingQTI.h"
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QTextDocument>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

class MyQTI: public QGraphicsTextItem {
public:
  MyQTI(QString t, QGraphicsItem *p=0): QGraphicsTextItem(t, p) { vis=true; }
  void setVis(bool v) { vis=v; update(); }
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget) {
    if (!vis)
      return;
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~(QStyle::State_Selected |QStyle::State_HasFocus);
    QGraphicsTextItem::paint(painter, &myOption, widget);
  }
private:
  bool vis;
}; 

DefaultingQTI::DefaultingQTI(QString dflttxt, QGraphicsItem *parent):
  QGraphicsTextItem(parent) {
  setFlag(QGraphicsTextItem::ItemIsFocusable);  
  dtext = new MyQTI(dflttxt, this);
  QFont f = font();
  f.setItalic(true);
  dtext->setFont(f);
  dtext->setDefaultTextColor(QColor("gray"));
  dtext->setFlag(QGraphicsTextItem::ItemIsFocusable);
  dtext->setFocusProxy(this);
  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(showOrHide()));
  reposition();
}

DefaultingQTI::~DefaultingQTI() {
}


void DefaultingQTI::paint(QPainter *painter,
			  const QStyleOptionGraphicsItem *option,
			  QWidget *widget) {
  QStyleOptionGraphicsItem myOption(*option);
  myOption.state &= ~(QStyle::State_Selected |QStyle::State_HasFocus);
  QGraphicsTextItem::paint(painter, &myOption, widget);
}

QRectF DefaultingQTI::inclusiveSceneBoundingRect() const {
  QRectF b0 = boundingRect();
  if (toPlainText().isEmpty())
    b0 |= dtext->mapRectToParent(dtext->boundingRect());
  return mapRectToScene(b0);
}

void DefaultingQTI::setDefaultText(QString s) {
  dtext->setPlainText(s);
  showOrHide();
}

void DefaultingQTI::focusInEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusInEvent(e);
  showOrHide();
}

void DefaultingQTI::focusOutEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusOutEvent(e);
  showOrHide();
}

void DefaultingQTI::showOrHide() {
  dtext->setVis(toPlainText().isEmpty() && !hasFocus());
}

void DefaultingQTI::setFont(QFont f) {
  QGraphicsTextItem::setFont(f);
  f.setItalic(true);
  dtext->setFont(f);
  reposition();
}

void DefaultingQTI::reposition() {
  QRectF r = dtext->boundingRect();
  dtext->setPos(-r.width()/2, 0);
}
