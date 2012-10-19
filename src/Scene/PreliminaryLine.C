// PreliminaryLine.C

#include "PreliminaryLine.H"
#include "Style.H"
#include <QBrush>
#include <QPen>

static QVariant style(QString s) {
  return Style::defaultStyle()[s];
}

PreliminaryLine::PreliminaryLine(QPointF p0, QGraphicsItem *parent):
  QGraphicsItem(parent), p0(p0) {
  line = new QGraphicsLineItem(this);
  line->setLine(QLineF(p0, p0));
  line->setPen(QPen(QBrush(QColor(style("preliminary-line-color").toString())),
		    style("preliminary-line-width").toDouble()));
  line->hide();
}

PreliminaryLine::~PreliminaryLine() {
}

void PreliminaryLine::updateEnd(QPointF p1_) {
  p1 = p1_;
  line->setLine(QLineF(p0, p1));
  if (isLine())
    line->show();
}

bool PreliminaryLine::isCircular() const {
  return false;
}

bool PreliminaryLine::isLine() const {
  return (p1-p0).manhattanLength() >= 5;
}

QPointF PreliminaryLine::startPoint() const {
  return p0;
}

QPointF PreliminaryLine::endPoint() const {
  return p1;
}

QRectF PreliminaryLine::boundingRect() const {
  return QRectF();
}

void PreliminaryLine::paint(QPainter *,
			    const QStyleOptionGraphicsItem *,
			    QWidget *) {
  // The "line" child does it all
  /* Note that we may use an ellipse rather than a line under certain
     circumstances in the future, hence the line as a private item
     rather than as a base class. */
}

