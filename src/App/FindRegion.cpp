// FindRegion.cpp

#include "FindRegion.h"
#include "TextItem.h"
#include <QPainter>

FindRegion::FindRegion(MarkupData *md, TextItem *item,
		       QGraphicsItem *parent): HoverRegion(md, item, parent) {
  c = item->style().color("search-result-background-color");
}

FindRegion::~FindRegion() {
}

void FindRegion::paint(QPainter *p,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  p->setPen(Qt::NoPen);
  p->setBrush(c);
  p->drawPath(boundingPath());
}
