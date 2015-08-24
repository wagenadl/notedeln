// DefaultingQTI.cpp

#include "DefaultingQTI.h"
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QTextDocument>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

DefaultingQTI::DefaultingQTI(QString dflttxt, QGraphicsItem *parent):
  QGraphicsTextItem(parent), dt(dflttxt) {
  connect(this, SIGNAL(contentsChange(int, int, int)), SLOT(update()));
  setPlainText(dt);
  setPlainText("");
}

DefaultingQTI::~DefaultingQTI() {
}

QRectF DefaultingQTI::boundingRect() const {
  QRectF r0 = QGraphicsTextItem::boundingRect();
  QFont f = font();
  f.setItalic(true);
  QFontMetricsF fm(f);
  QRectF r1 = fm.boundingRect(dt);
  QRectF r = r0 | r1.translated(r0.left() + r0.width()/2
				- r1.left() - r1.width()/2,
				r0.top() - r1.top());
  return r;
}

void DefaultingQTI::paint(QPainter *painter,
			  const QStyleOptionGraphicsItem *option,
			  QWidget *widget) {
  QStyleOptionGraphicsItem myOption(*option);
  myOption.state &= ~(QStyle::State_Selected |QStyle::State_HasFocus);
  QGraphicsTextItem::paint(painter, &myOption, widget);
  if (hasFocus() || !toPlainText().isEmpty())
    return;
  QFont f = font();
  f.setItalic(true);
  QFontMetricsF fm(f);
  QRectF r0 = QGraphicsTextItem::boundingRect();
  QRectF r1 = fm.boundingRect(dt);
  painter->setPen(QColor("gray"));
  painter->setFont(f);
  painter->drawText(r0.left() + r0.width()/2
		    - r1.left() - r1.width()/2, r0.top() - r1.top(),
		    dt);
}

void DefaultingQTI::setDefaultText(QString s) {
  prepareGeometryChange();
  dt = s;
  update();
}

void DefaultingQTI::focusInEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusInEvent(e);
  update();
}

void DefaultingQTI::focusOutEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusOutEvent(e);
  update();
}

void DefaultingQTI::setFont(QFont f) {
  prepareGeometryChange();
  QGraphicsTextItem::setFont(f);
}
